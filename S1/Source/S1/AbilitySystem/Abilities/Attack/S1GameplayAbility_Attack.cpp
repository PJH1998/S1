// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "S1LogChannels.h"
#include "AbilitySystem/Task/S1AbilityTask_RotateToCamera.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "Data/S1AnimData.h"
#include "System/S1AssetManager.h"
#include "Character/Player/S1Player.h"

#include "Weapon/S1Weapon.h"
#include "Components/BoxComponent.h"

#include "S1GameplayTags.h"
#include "Abilities/GameplayAbilityTargetTypes.h"

US1GameplayAbility_Attack::US1GameplayAbility_Attack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void US1GameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (UsedTag.IsValid())
	{
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(UsedTag);
	}

	if (bRotateToCamera)
	{
		StartRotateToCamera();
	}

	BindAttackBox();
}

void US1GameplayAbility_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UnbindAttackBox();
	HitTargets.Reset();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

const FS1MontageData* US1GameplayAbility_Attack::GetMontageData() const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (false == IsValid(AnimData))
	{
		return nullptr;
	}

	return AnimData->FindMontageByTag(MontageTag);
}

const FS1MontageSet* US1GameplayAbility_Attack::GetCurrentMontageSet() const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (false == IsValid(AnimData))
	{
		return nullptr;
	}

	return AnimData->FindMontageSet(MontageTag); // Index default = 0
}

void US1GameplayAbility_Attack::BindAttackBox()
{
	AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Player))
	{
		return;
	}

	AS1Weapon* Weapon = Player->GetEquippedWeapon();
	if (false == IsValid(Weapon))
	{
		return;
	}

	Weapon->OnHitCollisionEnabled.AddUObject(this, &ThisClass::ResetHitTargets);
	Weapon->GetAttackBox()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAttackBoxOverlap);
}

void US1GameplayAbility_Attack::UnbindAttackBox()
{
	AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Player))
	{
		return;
	}

	AS1Weapon* Weapon = Player->GetEquippedWeapon();
	if (false == IsValid(Weapon))
	{
		return;
	}

	Weapon->OnHitCollisionEnabled.RemoveAll(this);
	Weapon->GetAttackBox()->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnAttackBoxOverlap);
}

void US1GameplayAbility_Attack::ResetHitTargets()
{
	HitTargets.Reset();
}

void US1GameplayAbility_Attack::OnAttackBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (false == IsValid(OtherActor) || OtherActor == AvatarActor)
	{
		return;
	}

	for (const TWeakObjectPtr<AActor>& HitActor : HitTargets)
	{
		if (HitActor.Get() == OtherActor)
		{
			return;
		}
	}
	HitTargets.Add(OtherActor);

	LOG(TEXT("[AttackHit] Actor: %s | Channel: %d"),
		*OtherActor->GetName(), (int32)OtherComp->GetCollisionObjectType());

	const FS1MontageSet* MontageSet = GetCurrentMontageSet();
	if (nullptr == MontageSet)
	{
		return;
	}

	if (false == MontageSet->bAttackMontage)
	{
		LOG_WARNING(TEXT("US1GameplayAbility_Attack: MontageSet is not configured as AttackMontage. Check AnimData setup."));
		return;
	}

	if (nullptr == DamageEffect)
	{
		return;
	}

	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	const US1AttributeSet* AttribSet = Cast<US1AttributeSet>(OwnerASC->GetAttributeSet(US1AttributeSet::StaticClass()));
	if (nullptr == AttribSet)
	{
		return;
	}

	float FinalDamage = AttribSet->GetBaseDamage() * MontageSet->DamageRatio;

	LOG(TEXT("[AttackDamage] Target: %s | Base: %.1f | Ratio: %.2f | Final: %.1f"),
		*OtherActor->GetName(), AttribSet->GetBaseDamage(), MontageSet->DamageRatio, FinalDamage);

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	TargetData->HitResult.HitObjectHandle = FActorInstanceHandle(OtherActor);
	TargetDataHandle.Add(TargetData);

	// DamageEffect 항상 적용
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffect);
		SpecHandle.Data->SetSetByCallerMagnitude(S1SetByCallerTags::SetByCaller_Damage, -FinalDamage);
		ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle, TargetDataHandle);
	}

	// DebuffEffect 설정 시 추가 적용
	if (DebuffEffect)
	{
		FGameplayEffectSpecHandle DebuffSpecHandle = MakeOutgoingGameplayEffectSpec(DebuffEffect);
		ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), DebuffSpecHandle, TargetDataHandle);
	}
}

void US1GameplayAbility_Attack::StartRotateToCamera()
{
	if (IsValid(RotateTask))
	{
		// 태스크 재생성 없이 목표 Yaw만 갱신
		if (const APlayerController* PC = GetCurrentActorInfo()->PlayerController.Get())
		{
			RotateTask->UpdateTargetYaw(PC->GetControlRotation().Yaw);
		}
		return;
	}

	RotateTask = US1AbilityTask_RotateToCamera::RotateToCamera(this, RotationSpeed);
	RotateTask->ReadyForActivation();
}
