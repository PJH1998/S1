// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "S1LogChannels.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "AbilitySystem/Task/S1AbilityTask_RotateToCamera.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "System/S1AssetManager.h"
#include "Character/Player/S1Player.h"
#include "Weapon/S1Weapon.h"
#include "Components/BoxComponent.h"
#include "Tags/S1GameplayTags.h"
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

	if (IsValid(MontageProgression))
	{
		MontageProgression->Init(this);
		MontageProgression->OnActivated();
	}
}

void US1GameplayAbility_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UnbindAttackBox();
	HitTargets.Reset();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_Attack::OnProgressionMontageStarted()
{
	if (bRotateToCamera)
	{
		StartRotateToCamera();
	}
}

void US1GameplayAbility_Attack::ResetHitTargets()
{
	HitTargets.Reset();
}

void US1GameplayAbility_Attack::StartRotateToCamera()
{
	if (IsValid(RotateTask))
	{
		if (const APlayerController* PC = GetCurrentActorInfo()->PlayerController.Get())
		{
			RotateTask->UpdateTargetYaw(PC->GetControlRotation().Yaw);
		}
		return;
	}

	RotateTask = US1AbilityTask_RotateToCamera::RotateToCamera(this, RotationSpeed);
	RotateTask->ReadyForActivation();
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

void US1GameplayAbility_Attack::OnAttackBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	LOG(TEXT("[Overlap] OtherActor: %s | HitTargets: %d"),
		*OtherActor->GetName(), HitTargets.Num());

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

	AS1Player* DamagePlayer = Cast<AS1Player>(AvatarActor);
	AS1Weapon* DamageWeapon = IsValid(DamagePlayer) ? DamagePlayer->GetEquippedWeapon() : nullptr;

	const float        AtkScale       = IsValid(DamageWeapon) ? DamageWeapon->GetCurrentAtkScale()       : 1.0f;
	const FGameplayTag HitStrengthTag = IsValid(DamageWeapon) ? DamageWeapon->GetCurrentHitStrengthTag() : FGameplayTag();
	const float DmgMult               = IsValid(MontageProgression) ? MontageProgression->GetDamageMultiplier() : 1.0f;
	const float FinalDamage           = AttribSet->GetBaseDamage() * AtkScale * DmgMult;

	LOG(TEXT("[AttackDamage] Target: %s | Base: %.1f | AtkScale: %.2f | DmgMult: %.2f | Final: %.1f"),
		*OtherActor->GetName(), AttribSet->GetBaseDamage(), AtkScale, DmgMult, FinalDamage);

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	FHitResult DamageHitResult = SweepResult;
	DamageHitResult.HitObjectHandle = FActorInstanceHandle(OtherActor);
	DamageHitResult.Component = OtherComp;
	TargetData->HitResult = DamageHitResult;
	TargetDataHandle.Add(TargetData);

	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffect);
		SpecHandle.Data->GetContext().AddHitResult(DamageHitResult, true);
		SpecHandle.Data->SetSetByCallerMagnitude(S1SetByCallerTags::SetByCaller_Damage, -FinalDamage);
		if (HitStrengthTag.IsValid())
		{
			SpecHandle.Data->AppendDynamicAssetTags(FGameplayTagContainer(HitStrengthTag));
		}
		ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle, TargetDataHandle);
	}

	if (DebuffEffect)
	{
		FGameplayEffectSpecHandle DebuffSpecHandle = MakeOutgoingGameplayEffectSpec(DebuffEffect);
		ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), DebuffSpecHandle, TargetDataHandle);
	}
}
