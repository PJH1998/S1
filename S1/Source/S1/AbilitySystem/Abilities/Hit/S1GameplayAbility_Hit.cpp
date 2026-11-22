// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Hit/S1GameplayAbility_Hit.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "AbilitySystemComponent.h"
#include "Character/S1Character.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "S1LogChannels.h"
#include "Tags/S1GameplayTags.h"
#include "Animation/NotifyState/S1AnimNotifyState_HitLaunch.h"
#include "AbilitySystem/Task/S1AbilityTask_DelayedRootMotionForce.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"
#include "TimerManager.h"

US1GameplayAbility_Hit::US1GameplayAbility_Hit(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 피격 판정은 서버가 하지만 리액션 재생/넉백은 소유 클라에서도 돌아야 한다 — 몽타주가 엔진 복제
	// 경로(ASC->PlayMontage)를 타고, 넉백 루트모션도 예측 경로로 적용된다.
	// 넉백 데이터는 양쪽이 TriggerEventData->Instigator로 각자 계산하므로 전송 채널이 없다
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	// 이동/점프로 조기 탈출 불가 — Action 베이스의 EarlyMove 비활성화
	EarlyMoveEventTag = FGameplayTag();

	// 피격 리액션 중엔 항상 이동/점프 차단 — BP 설정에 맡기지 않고 강제 지정
	// (AS1Player::AddMovementInput/Jump가 ActionStateTag 보유 여부로 막는데, BP에서 안 채우면 몽타주 중에도 이동됨)
	ActionStateTag = S1StateTags::State_Action;
}

void US1GameplayAbility_Hit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (false == CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedLaunchVelocity = FVector::ZeroVector;
	bHitLaunchApplied    = false;

	AttackerActor = (nullptr != TriggerEventData) ? TriggerEventData->Instigator : nullptr;

	LOG(TEXT("[GA_Hit] %s ActivateAbility — Avatar: %s, Attacker: %s"),
		*GetClass()->GetName(),
		GetAvatarActorFromActorInfo() ? *GetAvatarActorFromActorInfo()->GetName() : TEXT("None"),
		AttackerActor.IsValid() ? *AttackerActor->GetName() : TEXT("None"));

	// 피격 시 진행중이던 액션 전부 중지 — 쿨다운은 GE라 CancelAllAbilities로는 안 끊김
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->CancelAllAbilities(this);
	}

	if (AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo()))
	{
		// 스프린트 등 잔존 속도/가속도 제거 — State.Action은 새 입력만 막고 기존 Velocity는 안 지워서
		// 정지형 피격(Weak)이 관성으로 미끄러지며 반응이 무시된 것처럼 보이는 문제 방지
		if (UCharacterMovementComponent* CMC = Character->GetCharacterMovement())
		{
			CMC->StopMovementImmediately();
		}

		// Assault 등 SpawnPersistentEffect로 띄운 이펙트가 몽타주 인터럽트로 DespawnEffect 노티파이를
		// 못 받아 영구 잔류하는 경우의 안전망 — 피격 시 보유 중인 이펙트 전부 강제 정리
		// ServerInitiated 하위(Weak/Death)는 소유 클라에서도 이 함수가 돈다 — 멀티캐스트는 서버에서만.
		// 클라가 NetMulticast를 부르면 경고 없이 로컬 실행되어(FunctionCallspace::Local) 중복된다
		if (HasAuthority(&ActivationInfo))
		{
			Character->MulticastClearAllAttachedEffects();
		}
	}
}

void US1GameplayAbility_Hit::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitLaunchFallingTimer);
	}

	// 넉백 중 이동 방향 재조준을 막으려고 걸어둔 오버라이드 해제 (CacheHitLaunch에서 Push)
	if (AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo()))
	{
		if (Character->IsHitLaunchOrientOverrideActive())
		{
			Character->PopOrientRotationOverride();
			Character->SetHitLaunchOrientOverrideActive(false);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_Hit::OnAbilityMontagePlayed(UAnimMontage* Montage, float Rate, FName SectionName)
{
	Super::OnAbilityMontagePlayed(Montage, Rate, SectionName);

	ApplyHitLaunchRootMotion(Montage, Rate);
}

void US1GameplayAbility_Hit::CacheHitLaunch(const FVector& InLaunchVelocity, const FVector& InFacingDirection)
{
	CachedLaunchVelocity = InLaunchVelocity;

	AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		return;
	}

	// 회전 스냅과 Orient 오버라이드를 활성화 시점에 처리한다 — 노티파이에서 하면 데디 서버 thrash가
	// Push/Pop을 매 프레임 흔든다. 시뮬 프록시는 이 회전을 복제로 받으므로 전체 머신 적용이 불필요하다
	if (false == InFacingDirection.IsNearlyZero())
	{
		Character->SetActorRotation(InFacingDirection.Rotation());
	}

	if (false == Character->IsHitLaunchOrientOverrideActive())
	{
		Character->PushOrientRotationOverride();
		Character->SetHitLaunchOrientOverrideActive(true);
	}
}

void US1GameplayAbility_Hit::ApplyHitLaunchRootMotion(UAnimMontage* Montage, float Rate)
{
	if (nullptr == Montage || bHitLaunchApplied || CachedLaunchVelocity.IsNearlyZero())
	{
		return;
	}

	// HitLaunch 노티파이를 데이터 마커로 스캔 (US1GameplayAbility_Action::ApplyMontageRootMotion과 동일 패턴)
	float StartTime         = -1.f;
	float WindowDuration    = 0.f;
	bool  bMaintainOnFinish = false;
	for (const FAnimNotifyEvent& Event : Montage->Notifies)
	{
		if (const US1AnimNotifyState_HitLaunch* LaunchNotify = Cast<US1AnimNotifyState_HitLaunch>(Event.NotifyStateClass))
		{
			StartTime         = Event.GetTriggerTime();
			WindowDuration    = Event.GetDuration();
			bMaintainOnFinish = LaunchNotify->ShouldMaintainVelocityOnFinish();
			break;
		}
	}

	// 넉백 노티파이 없는 몽타주(Weak 등) = 넉백 없음
	if (StartTime < 0.f || WindowDuration <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	// 몽타주는 Rate × RateScale 배속으로 재생 → 루트모션 태스크 기준인 real-time으로 환산 (§03)
	const float EffectiveRate = FMath::Max(Rate * Montage->RateScale, KINDA_SMALL_NUMBER);
	const float RealWindup    = StartTime / EffectiveRate;
	const float RealWindow    = WindowDuration / EffectiveRate;

	// 힘이 시작되는 순간에 Falling으로. GA 타이머로 하는 이유는 EnterFallingForHitLaunch 주석 참조
	if (RealWindup <= KINDA_SMALL_NUMBER)
	{
		EnterFallingForHitLaunch();
	}
	else if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			HitLaunchFallingTimer,
			FTimerDelegate::CreateWeakLambda(this, [this]() { EnterFallingForHitLaunch(); }),
			RealWindup,
			false);
	}

	US1AbilityTask_DelayedRootMotionForce* LaunchTask = US1AbilityTask_DelayedRootMotionForce::ApplyDelayedRootMotionConstantForce(
		this,
		US1AnimNotifyState_HitLaunch::RootMotionSourceName,
		CachedLaunchVelocity.GetSafeNormal(),
		CachedLaunchVelocity.Size(),
		RealWindup + RealWindow,
		RealWindup,								// 이 시간 전에는 이동 없음
		bMaintainOnFinish ? ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity
						  : ERootMotionFinishVelocityMode::SetVelocity,
		FVector::ZeroVector,					// SetVelocityOnFinish
		0.f,									// ClampVelocityOnFinish
		false);									// bEnableGravity=false — 넉백은 Z도 루트모션이 가져야 한다(IgnoreZAccumulate 끔)
	if (nullptr == LaunchTask)
	{
		return;
	}

	LaunchTask->ReadyForActivation();
	bHitLaunchApplied = true;
}

void US1GameplayAbility_Hit::EnterFallingForHitLaunch()
{
	// 노티파이가 아니라 GA 타이머/활성화 스코프에서 하는 이유: 데디 서버 NotifyState thrash가
	// Begin/End를 매 프레임 재발화시켜 NotifyEnd가 소스를 철거해버렸다(로그로 확정, §04와 동일 원인)
	AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character))
	{
		return;
	}

	UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
	if (nullptr == CMC)
	{
		return;
	}

	if (MOVE_Falling != CMC->MovementMode)
	{
		CMC->SetMovementMode(MOVE_Falling);
	}
}

void US1GameplayAbility_Hit::ComputeKnockback(float LaunchSpeed, bool& bOutHitFromBack, FVector& OutHitDirection, FVector& OutLaunchVelocity) const
{
	const AActor* Character = GetAvatarActorFromActorInfo();

	FVector HitDir = IsValid(Character) ? Character->GetActorForwardVector() : FVector::ForwardVector;
	if (AttackerActor.IsValid() && IsValid(Character))
	{
		FVector ToAttacker = AttackerActor->GetActorLocation() - Character->GetActorLocation();
		ToAttacker.Z = 0.f;
		if (false == ToAttacker.IsNearlyZero())
		{
			HitDir = ToAttacker.GetSafeNormal();
		}
	}

	bOutHitFromBack = IsValid(Character) && FVector::DotProduct(Character->GetActorForwardVector(), HitDir) < 0.f;

	// 공격원을 향하는 방향(정면 스냅 여부는 호출부가 bOutHitFromBack으로 직접 판단)
	OutHitDirection = HitDir;

	// 회전 분기와 무관하게 항상 공격원 반대 방향(월드 기준)
	OutLaunchVelocity = -HitDir * LaunchSpeed;
}

AS1Character* US1GameplayAbility_Hit::GetValidCharacterOrEnd(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo());
	if (false == IsValid(Character) || false == IsValid(MontageProgression))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return nullptr;
	}
	return Character;
}

void US1GameplayAbility_Hit::StartMontageProgression()
{
	MontageProgression->Init(this);
	MontageProgression->OnActivated();
}
