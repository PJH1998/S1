// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/S1Character.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"

// Sets default values
AS1Character::AS1Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AS1Character::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AS1Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAbilitySystemComponent* AS1Character::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AS1Character::InitSystem()
{
}

void AS1Character::MulticastPlayMontage_Implementation(UAnimMontage* Montage, float Rate, FName StartSection)
{
	if (nullptr == Montage)
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = GetMesh();
	UAnimInstance* AnimInst = IsValid(MeshComp) ? MeshComp->GetAnimInstance() : nullptr;
	if (nullptr == AnimInst)
	{
		return;
	}

	AnimInst->Montage_Play(Montage, Rate);

	if (StartSection != NAME_None)
	{
		AnimInst->Montage_JumpToSection(StartSection, Montage);
	}
}

void AS1Character::MulticastJumpToSection_Implementation(UAnimMontage* Montage, FName SectionName)
{
	if (nullptr == Montage || SectionName.IsNone())
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = GetMesh();
	UAnimInstance* AnimInst = IsValid(MeshComp) ? MeshComp->GetAnimInstance() : nullptr;
	if (nullptr == AnimInst || false == AnimInst->Montage_IsPlaying(Montage))
	{
		return;
	}

	AnimInst->Montage_JumpToSection(SectionName, Montage);
}

void AS1Character::SetReplicatedGravityScale(float Scale)
{
	// 서버 권위 — 즉시 서버 CMC 적용 + 복제 트리거(소유 클라 포함 OnRep에서 적용)
	RepGravityScale = Scale;
	if (UCharacterMovementComponent* CMC = GetCharacterMovement())
	{
		CMC->GravityScale = Scale;
	}
}

void AS1Character::OnRep_GravityScale()
{
	if (UCharacterMovementComponent* CMC = GetCharacterMovement())
	{
		CMC->GravityScale = RepGravityScale;
	}
}

void AS1Character::MulticastSetHitLaunchData_Implementation(FVector Velocity, FVector Facing)
{
	PendingHitLaunchVelocity  = Velocity;
	PendingHitFacingDirection = Facing;
}

void AS1Character::RegisterAttachedEffect(FGameplayTag EffectKey, UNiagaraComponent* Component)
{
	if (false == EffectKey.IsValid() || false == ::IsValid(Component))
	{
		return;
	}

	ActiveAttachedEffects.FindOrAdd(EffectKey).Add(Component);
}

void AS1Character::EndAttachedEffect(FGameplayTag EffectKey, float LifetimeAfterEnd)
{
	TArray<TWeakObjectPtr<UNiagaraComponent>>* FoundArray = ActiveAttachedEffects.Find(EffectKey);
	if (nullptr == FoundArray || 0 == FoundArray->Num())
	{
		return;
	}

	// 가장 먼저 등록된(Begin이 가장 먼저 호출된) 인스턴스부터 처리 — FIFO
	TWeakObjectPtr<UNiagaraComponent> OldestEntry = (*FoundArray)[0];
	FoundArray->RemoveAt(0);

	UNiagaraComponent* Component = OldestEntry.Get();
	if (nullptr == Component)
	{
		return;
	}

	// 현재 부착돼 있는지는 컴포넌트 스스로 알 수 있으므로 호출부가 Attach 여부를 몰라도 됨
	if (nullptr != Component->GetAttachParent())
	{
		Component->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}

	if (LifetimeAfterEnd > 0.f)
	{
		TWeakObjectPtr<UNiagaraComponent> WeakComponent = Component;
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([WeakComponent]()
		{

			if (UNiagaraComponent* TimedOutComponent = WeakComponent.Get())
			{
				TimedOutComponent->Deactivate();
			}
		}), LifetimeAfterEnd, false);
	}
	else
	{
		// 0 이하면 대기 없이 즉시 Deactivate — NS 자체 Loop/Duration 설정에 기대지 않고 확실히 스폰 중단
		Component->Deactivate();
	}
}

const TArray<TWeakObjectPtr<UNiagaraComponent>>* AS1Character::FindAttachedEffects(FGameplayTag EffectKey) const
{
	return ActiveAttachedEffects.Find(EffectKey);
}

void AS1Character::ArmEffectAutoDestroy(UNiagaraComponent* Component)
{
	if (::IsValid(Component))
	{
		Component->OnSystemFinished.AddUniqueDynamic(this, &AS1Character::OnManagedEffectFinished);
	}
}

void AS1Character::OnManagedEffectFinished(UNiagaraComponent* FinishedComponent)
{
	if (::IsValid(FinishedComponent))
	{
		FinishedComponent->DestroyComponent();
	}
}

void AS1Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS1Character, RepGravityScale);
}
