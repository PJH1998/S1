// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
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
#include "Animation/NotifyState/S1AnimNotifyState_AtkCollision.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"
#include "TimerManager.h"

US1GameplayAbility_Attack::US1GameplayAbility_Attack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 모든 공격은 RootMotion/노티파이 이동을 포함 → 소유 클라 예측 필요 (§40 이동 GA 전환)
	// ⚠️ BP 저장값이 C++을 덮으므로 각 공격 BP Class Defaults에도 Local Predicted 설정 필요
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
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
	ClearHitWindowTimers();
	DisableWeaponHitCollision(ES1AttackHand::Main);
	DisableWeaponHitCollision(ES1AttackHand::Offhand);
	UnbindAttackBox();
	MainHitTargets.Reset();
	OffhandHitTargets.Reset();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_Attack::OnProgressionMontageStarted()
{
	if (bRotateToCamera)
	{
		StartRotateToCamera();
	}
}

void US1GameplayAbility_Attack::OnAbilityMontagePlayed(UAnimMontage* Montage, float Rate)
{
	// 새 몽타주(스윙) 진입 — 이전 윈도우 정리 후 이 몽타주의 AtkCollision 윈도우를 서버에서 스케줄
	ClearHitWindowTimers();
	DisableWeaponHitCollision(ES1AttackHand::Main);
	DisableWeaponHitCollision(ES1AttackHand::Offhand);
	ScheduleHitWindows(Montage, Rate);
}

void US1GameplayAbility_Attack::ScheduleHitWindows(UAnimMontage* Montage, float Rate)
{
	// 데미지는 서버 권위 → 히트 윈도우도 서버에서만 구동 (클라 노티파이는 데이터 마커로 무동작)
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (false == IsValid(Avatar) || false == Avatar->HasAuthority())
	{
		return;
	}

	UWorld* World = Avatar->GetWorld();
	if (nullptr == Montage || nullptr == World)
	{
		return;
	}

	// 몽타주는 Rate × RateScale 배속 재생 → montage-time을 real-time으로 변환 (÷ 배속)
	const float EffectiveRate = FMath::Max(Rate * Montage->RateScale, KINDA_SMALL_NUMBER);

	for (const FAnimNotifyEvent& Event : Montage->Notifies)
	{
		const US1AnimNotifyState_AtkCollision* AtkNotify = Cast<US1AnimNotifyState_AtkCollision>(Event.NotifyStateClass);
		if (nullptr == AtkNotify)
		{
			continue;
		}

		const float           RealBegin  = Event.GetTriggerTime() / EffectiveRate;
		const float           RealEnd    = (Event.GetTriggerTime() + Event.GetDuration()) / EffectiveRate;
		const float           AtkScale   = AtkNotify->GetAtkScale();
		const FGameplayTag    StrengthTag = AtkNotify->GetHitStrengthTag();
		const ES1AttackHand   Hand       = AtkNotify->GetAttackHand();

		// Begin
		FTimerHandle BeginHandle;
		World->GetTimerManager().SetTimer(
			BeginHandle,
			FTimerDelegate::CreateWeakLambda(this, [this, AtkScale, StrengthTag, Hand]()
			{
				EnableWeaponHitCollision(AtkScale, StrengthTag, Hand);
			}),
			FMath::Max(RealBegin, 0.001f), false);
		HitWindowTimers.Add(BeginHandle);

		// End
		FTimerHandle EndHandle;
		World->GetTimerManager().SetTimer(
			EndHandle,
			FTimerDelegate::CreateWeakLambda(this, [this, Hand]()
			{
				DisableWeaponHitCollision(Hand);
			}),
			FMath::Max(RealEnd, 0.002f), false);
		HitWindowTimers.Add(EndHandle);
	}
}

void US1GameplayAbility_Attack::ClearHitWindowTimers()
{
	if (UWorld* World = GetWorld())
	{
		for (FTimerHandle& Handle : HitWindowTimers)
		{
			World->GetTimerManager().ClearTimer(Handle);
		}
	}
	HitWindowTimers.Reset();
}

void US1GameplayAbility_Attack::EnableWeaponHitCollision(float AtkScale, FGameplayTag HitStrengthTag, ES1AttackHand Hand)
{
	AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Player))
	{
		return;
	}

	if (Hand == ES1AttackHand::Main)
	{
		if (AS1Weapon* Weapon = Player->GetEquippedWeapon())
		{
			Weapon->EnableHitCollision(AtkScale, HitStrengthTag);
		}
	}
	else
	{
		if (AS1Weapon* Offhand = Player->GetEquippedOffhandWeapon())
		{
			Offhand->EnableHitCollision(AtkScale, HitStrengthTag);
		}
	}
}

void US1GameplayAbility_Attack::DisableWeaponHitCollision(ES1AttackHand Hand)
{
	AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Player))
	{
		return;
	}

	if (Hand == ES1AttackHand::Main)
	{
		if (AS1Weapon* Weapon = Player->GetEquippedWeapon())
		{
			Weapon->DisableHitCollision();
		}
	}
	else
	{
		if (AS1Weapon* Offhand = Player->GetEquippedOffhandWeapon())
		{
			Offhand->DisableHitCollision();
		}
	}
}

void US1GameplayAbility_Attack::ResetMainHitTargets()
{
	MainHitTargets.Reset();
}

void US1GameplayAbility_Attack::ResetOffhandHitTargets()
{
	OffhandHitTargets.Reset();
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

	if (AS1Weapon* Weapon = Player->GetEquippedWeapon())
	{
		Weapon->OnHitCollisionEnabled.AddUObject(this, &ThisClass::ResetMainHitTargets);
		Weapon->GetAttackBox()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAttackBoxOverlap);
	}

	if (AS1Weapon* Offhand = Player->GetEquippedOffhandWeapon())
	{
		Offhand->OnHitCollisionEnabled.AddUObject(this, &ThisClass::ResetOffhandHitTargets);
		Offhand->GetAttackBox()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAttackBoxOverlap);
	}
}

void US1GameplayAbility_Attack::UnbindAttackBox()
{
	AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Player))
	{
		return;
	}

	if (AS1Weapon* Weapon = Player->GetEquippedWeapon())
	{
		Weapon->OnHitCollisionEnabled.RemoveAll(this);
		Weapon->GetAttackBox()->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnAttackBoxOverlap);
	}

	if (AS1Weapon* Offhand = Player->GetEquippedOffhandWeapon())
	{
		Offhand->OnHitCollisionEnabled.RemoveAll(this);
		Offhand->GetAttackBox()->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnAttackBoxOverlap);
	}
}

void US1GameplayAbility_Attack::OnAttackBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 데미지 적용은 서버 권위에서만
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (false == IsValid(AvatarActor) || false == AvatarActor->HasAuthority())
	{
		return;
	}

	if (false == IsValid(OtherActor) || OtherActor == AvatarActor)
	{
		return;
	}

	// 어느 손의 AttackBox가 발동했는지 판별 → 각 손의 HitTargets 독립 관리
	AS1Player* DamagePlayer = Cast<AS1Player>(AvatarActor);
	AS1Weapon* OffhandWeapon = IsValid(DamagePlayer) ? DamagePlayer->GetEquippedOffhandWeapon() : nullptr;
	const bool bIsOffhand = IsValid(OffhandWeapon) && OverlappedComp == OffhandWeapon->GetAttackBox();
	TArray<TWeakObjectPtr<AActor>>& Targets = bIsOffhand ? OffhandHitTargets : MainHitTargets;

	for (const TWeakObjectPtr<AActor>& HitActor : Targets)
	{
		if (HitActor.Get() == OtherActor)
		{
			return;
		}
	}
	Targets.Add(OtherActor);

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

	AS1Weapon* DamageWeapon = bIsOffhand ? OffhandWeapon : (IsValid(DamagePlayer) ? DamagePlayer->GetEquippedWeapon() : nullptr);

	const float        AtkScale       = IsValid(DamageWeapon) ? DamageWeapon->GetCurrentAtkScale()       : 1.0f;
	const FGameplayTag HitStrengthTag = IsValid(DamageWeapon) ? DamageWeapon->GetCurrentHitStrengthTag() : FGameplayTag();
	const float DmgMult               = IsValid(MontageProgression) ? MontageProgression->GetDamageMultiplier() : 1.0f;
	const float FinalDamage           = AttribSet->GetBaseDamage() * AtkScale * DmgMult;

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
