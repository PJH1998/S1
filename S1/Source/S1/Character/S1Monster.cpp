// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/S1Monster.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "AI/S1AIController.h"
#include "Animation/S1AnimInstance_BossBase.h"
#include "AIController.h"
#include "Component/S1DeathPresentationComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"

AS1Monster::AS1Monster()
	: Super()
{
	AbilitySystemComponent = CreateDefaultSubobject<US1AbilitySystemComponent>("AbilitySystemComponent");
	DeathPresentationComponent = CreateDefaultSubobject<US1DeathPresentationComponent>(TEXT("DeathPresentationComponent"));
}

void AS1Monster::BeginPlay()
{
	Super::BeginPlay();
	InitSystem();
}

void AS1Monster::PlayAnimation(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}

// HP 0: bIsDeadAnim·물리·BT 정지. 데스 재생·연출 타이밍은 ABP + AnimNotify.
void AS1Monster::NotifyDeath()
{
	if (bIsDead)
	{
		return;
	}
	bIsDead = true;
	if (USkeletalMeshComponent* SkeletalMeshComp = GetMesh())
	{
		if (US1AnimInstance_BossBase* BossAnimInstance = Cast<US1AnimInstance_BossBase>(SkeletalMeshComp->GetAnimInstance()))
		{
			BossAnimInstance->SetDeadAnimState(true);
		}
	}
	if (US1AbilitySystemComponent* ASC = Cast<US1AbilitySystemComponent>(AbilitySystemComponent))
	{
		ASC->CancelAllAbilities();
	}
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
		if (AS1AIController* S1AIController = Cast<AS1AIController>(AIController))
		{
			S1AIController->SetBlackboardIsDead(true);
		}
	}
	HandleDeathPrepare();

	if (AS1AIController* S1AIController = Cast<AS1AIController>(GetController()))
	{
		S1AIController->StopAIForDeath();
	}
}

void AS1Monster::HandleDeathPrepare()
{
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AS1Monster::OnDeathAnimSequenceEnded()
{
	if (bIsDead == false)
	{
		return;
	}

	FinalizeDeath();
}

void AS1Monster::OnDeathAnimationFinished()
{
	OnDeathAnimSequenceEnded();
	BeginDeathPresentation();
}

void AS1Monster::FreezeDeathPose(UAnimMontage* MontageToStop)
{
	if (bDeathPoseFrozen)
	{
		return;
	}

	bDeathPoseFrozen = true;

	if (USkeletalMeshComponent* SkeletalMeshComp = GetMesh())
	{
		if (UAnimInstance* AnimInstance = SkeletalMeshComp->GetAnimInstance())
		{
			if (MontageToStop)
			{
				AnimInstance->Montage_Stop(0.f, MontageToStop);
			}
			else if (UAnimMontage* ActiveMontage = AnimInstance->GetCurrentActiveMontage())
			{
				AnimInstance->Montage_Stop(0.f, ActiveMontage);
			}
		}

		SkeletalMeshComp->bPauseAnims = true;
	}
}

void AS1Monster::FinalizeDeath()
{
	FreezeDeathPose();
}

void AS1Monster::BeginDeathPresentation()
{
	if (bDeathPresentationStarted || bIsDead == false)
	{
		return;
	}
	bDeathPresentationStarted = true;
	UnbindDeathPresentation();
	if (US1DeathPresentationComponent* DeathPresentation = GetDeathPresentationComponent())
	{
		DeathPresentation->OnPresentationComplete.AddDynamic(this, &AS1Monster::HandleDeathPresentationFinished);
		DeathPresentation->StartPresentation();
		return;
	}
	OnDeathPresentationComplete();
}

// 연출 완료 델리게이트 수신용. OnDeathPresentationComplete로 넘긴다.
void AS1Monster::HandleDeathPresentationFinished()
{
	OnDeathPresentationComplete();
}

// 연출 종료: 델리게이트 해제 후 bUsePooling이면 풀 반환(Destroy 없음).
void AS1Monster::OnDeathPresentationComplete()
{
	UnbindDeathPresentation();
	if (bUsePooling)
	{
		ReturnSelf();
	}
}

// 보스 등 같은 액터 리스폰: 사망 상태·연출·AI·스탯을 살아 있는 상태로 되돌린다.
void AS1Monster::ReviveForRespawn()
{
	if (bIsDead == false)
	{
		return;
	}
	UnbindDeathPresentation();
	if (US1DeathPresentationComponent* DeathPresentation = GetDeathPresentationComponent())
	{
		DeathPresentation->StopPresentation();
	}
	RestoreAliveState();
	if (AS1AIController* S1AIController = Cast<AS1AIController>(GetController()))
	{
		S1AIController->ResetBlackboardForSpawn();
		S1AIController->ResumeAIAfterRevive();
	}
}

// 풀에서 꺼낼 때(IS1PoolingInterface). 위치/표시 복구 후 ResetForPoolSpawn.
void AS1Monster::OnSpawnFromPool(FGameplayTag InPoolTag, FVector Location, FRotator Rotation)
{
	IS1PoolingInterface::OnSpawnFromPool(InPoolTag, Location, Rotation);
	ResetForPoolSpawn();
}

// 풀에 넣을 때(IS1PoolingInterface). 연출 중단 후 Hidden·충돌 OFF.
void AS1Monster::OnReturnToPool()
{
	UnbindDeathPresentation();
	if (US1DeathPresentationComponent* DeathPresentation = GetDeathPresentationComponent())
	{
		DeathPresentation->StopPresentation();
	}
	IS1PoolingInterface::OnReturnToPool();
}

// 풀 재사용 직전: 살아 있는 상태 복구 + AI 블랙보드/BT 재개.
void AS1Monster::ResetForPoolSpawn()
{
	RestoreAliveState();
	if (AS1AIController* S1AIController = Cast<AS1AIController>(GetController()))
	{
		S1AIController->ResetBlackboardForSpawn();
		S1AIController->ResumeAIAfterRevive();
	}
}

// bIsDead 해제, 체력·이동·캡슐·스켈레탈 애니를 전투 가능 상태로 복원.
void AS1Monster::RestoreAliveState()
{
	bIsDead = false;
	bDeathPoseFrozen = false;
	bDeathPresentationStarted = false;
	SetActorHiddenInGame(false);
	if (US1AttributeSet* MonsterAttributeSet = Cast<US1AttributeSet>(AttributeSet))
	{
		MonsterAttributeSet->SetHealth(MonsterAttributeSet->GetMaxHealth());
	}
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->SetMovementMode(MOVE_Walking);
	}
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	if (USkeletalMeshComponent* SkeletalMeshComp = GetMesh())
	{
		SkeletalMeshComp->bPauseAnims = false;
		if (UAnimInstance* AnimInstance = SkeletalMeshComp->GetAnimInstance())
		{
			if (US1AnimInstance_BossBase* BossAnimInstance = Cast<US1AnimInstance_BossBase>(AnimInstance))
			{
				BossAnimInstance->SetDeadAnimState(false);
				BossAnimInstance->ResetLocomotion();
			}
		}
	}
}

// DeathPresentation 완료 델리게이트 바인딩 해제(풀/리스폰/재사망 시 중복 방지).
void AS1Monster::UnbindDeathPresentation()
{
	if (US1DeathPresentationComponent* DeathPresentation = GetDeathPresentationComponent())
	{
		DeathPresentation->OnPresentationComplete.RemoveDynamic(this, &AS1Monster::HandleDeathPresentationFinished);
	}
}

void AS1Monster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AS1Monster::InitSystem()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

ES1EnemyTier AS1Monster::GetLockOnTier_Implementation()
{
	return ES1EnemyTier::Normal;
}

FVector AS1Monster::GetLockOnFocusLocation_Implementation()
{
	USkeletalMeshComponent* SkelMesh = GetMesh();
	if (false == ::IsValid(SkelMesh) || SkelMesh->GetBoneIndex(LockOnFocusBone) == INDEX_NONE)
	{
		return GetActorLocation();
	}
	return SkelMesh->GetBoneLocation(LockOnFocusBone);
}
