// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Tags/GAS/S1GameplayTags_GAS.h"
#include "AbilitySystem/Task/S1AbilityTask_EarlyExitChecker.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "Curves/CurveFloat.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/S1AnimInstance.h"
#include "Animation/NotifyState/S1AnimNotifyState_MoveEvent.h"
#include "Character/S1Character.h"
#include "Character/Player/S1Player.h"

US1GameplayAbility_Action::US1GameplayAbility_Action(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActionStateTag    = S1StateTags::State_Action;
	MoveBeginEventTag = S1EventTags::Event_MovementStart;
	MoveEndEventTag   = S1EventTags::Event_MovementEnd;
	EarlyMoveEventTag = S1EventTags::Event_EarlyMove;
	JumpEventTag      = S1EventTags::Event_Jump;
}

void US1GameplayAbility_Action::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 액션 중 상태 태그 부여 — Jump()/AddMovementInput() 차단 조건
	// ServerOnly GA라 서버에서만 부여 → 클라의 이동/점프 검사가 보도록 TagOnly로 복제
	if (ActionStateTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->AddLooseGameplayTag(ActionStateTag, 1, EGameplayTagReplicationState::TagOnly);
		}
	}

	// 이동 방향을 활성화 시점에 1회 캡처 (컨트롤 회전=카메라 방향, 복제되어 클라/서버 일치)
	if (const APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
	{
		FVector Dir = Pawn->GetControlRotation().Vector();
		Dir.Z = 0.f;
		if (false == Dir.IsNearlyZero())
		{
			CapturedActionMoveDir = Dir.GetSafeNormal();
		}
		else
		{
			CapturedActionMoveDir = Pawn->GetActorForwardVector();
		}
	}

	// EarlyMove 이벤트 대기 등록
	if (EarlyMoveEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EarlyMoveEventTag);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnEarlyMoveEventReceived);
		Task->ReadyForActivation();
	}

	// Move 이벤트 바인딩 — 서브클래스 virtual dispatch를 위해 non-virtual 래퍼 사용
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (MoveBeginEventTag.IsValid())
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(MoveBeginEventTag)
				.AddUObject(this, &ThisClass::InternalMoveBeginCallback);
		}
		if (MoveEndEventTag.IsValid())
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(MoveEndEventTag)
				.AddUObject(this, &ThisClass::InternalMoveEndCallback);
		}
	}
}

void US1GameplayAbility_Action::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActionStateTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			if (ASC->HasMatchingGameplayTag(ActionStateTag))
			{
				ASC->RemoveLooseGameplayTag(ActionStateTag, 1, EGameplayTagReplicationState::TagOnly);
			}
		}
	}

	if (IsValid(MontageProgression))
	{
		MontageProgression->OnDeactivated();
	}

	// 안전망 — MoveEnd 이벤트 없이 종료된 경우 대비
	if (IsValid(MoveTask))
	{
		MoveTask->EndTask();
		MoveTask = nullptr;
	}

	// Move 이벤트 바인딩 해제
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (MoveBeginEventTag.IsValid())
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(MoveBeginEventTag).RemoveAll(this);
		}
		if (MoveEndEventTag.IsValid())
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(MoveEndEventTag).RemoveAll(this);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool US1GameplayAbility_Action::OnInputReactivated()
{
	if (IsValid(MontageProgression))
	{
		// 콤보 어드밴스 시 이동 방향 재캡처 — RotateToCamera로 캐릭터가 매 타 재조준되므로
		// 루트모션도 첫 타 방향이 아닌 현재 조준 방향을 따라가야 함 (컨트롤 회전=복제되어 클라/서버 일치)
		if (const APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
		{
			FVector Dir = Pawn->GetControlRotation().Vector();
			Dir.Z = 0.f;
			if (false == Dir.IsNearlyZero())
			{
				CapturedActionMoveDir = Dir.GetSafeNormal();
			}
		}
		return MontageProgression->OnInputReactivated();
	}
	return false;
}

void US1GameplayAbility_Action::OnInputReleased()
{
	if (IsValid(MontageProgression))
	{
		MontageProgression->OnInputReleased();
	}
}

bool US1GameplayAbility_Action::OnCrossInput(const FGameplayTagContainer& TargetAbilityTags)
{
	if (IsValid(MontageProgression))
	{
		return MontageProgression->OnCrossInput(TargetAbilityTags);
	}
	return false;
}

FGameplayTag US1GameplayAbility_Action::GetInputFlushTag() const
{
	if (IsValid(MontageProgression))
	{
		return MontageProgression->GetInputFlushTag();
	}
	return FGameplayTag();
}

void US1GameplayAbility_Action::RequestEndAbility(bool bWasCancelled)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}

void US1GameplayAbility_Action::RequestReactivateSelf()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (false == IsValid(ASC))
	{
		return;
	}

	FGameplayTagContainer OwnTags;
	if (const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		OwnTags = Spec->Ability->AbilityTags;
	}

	if (false == OwnTags.IsValid())
	{
		return;
	}

	// EndAbility 전에 재발동 가능 여부 확인 — Blocked/Required Tag 불충족 시 현재 GA 유지
	// (ex. 공중 콤보 UsedTag — 확인 없이 끝내면 몽타주 진행 중 GravityScale 등이 복구됨)
	TArray<FGameplayAbilitySpec*> ActivatableSpecs;
	ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(OwnTags, ActivatableSpecs);
	if (ActivatableSpecs.IsEmpty())
	{
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	ASC->TryActivateAbilitiesByTag(OwnTags);
}

void US1GameplayAbility_Action::RequestActivateAbilityByTag(const FGameplayTagContainer& Tags)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->TryActivateAbilitiesByTag(Tags);
	}
}

US1AnimInstance* US1GameplayAbility_Action::GetAnimInstanceForProgression() const
{
	return GetAnimInstance();
}

float US1GameplayAbility_Action::PlayAbilityMontage(UAnimMontage* Montage, FName StartSection, float Rate)
{
	if (nullptr == Montage)
	{
		return 0.f;
	}

	AS1Character* Character = Cast<AS1Character>(GetAvatarActorFromActorInfo());
	if (false == ::IsValid(Character))
	{
		return 0.f;
	}

	// 서브클래스 훅 — GA_Attack이 이 몽타주의 AtkCollision 노티파이를 스캔해 서버에서 히트 윈도우 타이머 스케줄
	// (서버 노티파이 per-frame thrash 우회: 노티파이는 데이터 마커, 실제 on/off는 GA가 1회씩 구동)
	OnAbilityMontagePlayed(Montage, Rate);

	// LocalPredicted: GAS 네이티브 예측 몽타주 — 소유 클라는 예측 재생(1회), 서버는 RepAnimMontage로 시뮬프록시에만 복제(소유자 스킵)
	// MulticastPlayMontage를 쓰면 소유 클라가 (예측 재생 + 서버 멀티캐스트) 이중 재생 → 몽타주 재시작으로 노티파이 NotifyEnd 조기 발화(MoveBegin/End 붕괴)
	if (EGameplayAbilityNetExecutionPolicy::LocalPredicted == GetNetExecutionPolicy())
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (nullptr == ASC)
		{
			return 0.f;
		}

		const float Duration = ASC->PlayMontage(this, GetCurrentActivationInfo(), Montage, Rate, StartSection);

		// 예측 스코프(ActivateAbility 호출 스택) 안에서 루트모션 생성 — 몽타주의 MoveEvent 노티파이를 데이터로 읽음
		// 노티파이에서 직접 만들면 클라/서버 독립 발화라 공유 예측키가 없어 서버에 네트워킹 안 됨 (틱/스냅백)
		ApplyMontageRootMotion(Montage, Rate);

		return Duration;
	}

	// ServerOnly — 서버 권위 GA의 몽타주를 전체 클라(소유자 포함)+서버에서 재생
	// 서버는 동기 실행되어 이어지는 Montage_SetEndDelegate/섹션 제어가 서버 AnimInstance에 정상 바인딩
	Character->MulticastPlayMontage(Montage, Rate, StartSection);

	// 실패 판정용 길이 반환 (실제 종료는 서버 측 종료 델리게이트가 처리)
	return Montage->GetPlayLength();
}

FVector US1GameplayAbility_Action::GetCapturedMoveDirection() const
{
	return CapturedActionMoveDir;
}

void US1GameplayAbility_Action::ApplyMontageRootMotion(UAnimMontage* Montage, float Rate)
{
	// 예측 어빌리티만 — 예측키가 있어야 루트모션이 서버까지 네트워킹됨 (ServerOnly는 notify 직접 경로 사용)
	if (EGameplayAbilityNetExecutionPolicy::LocalPredicted != GetNetExecutionPolicy())
	{
		return;
	}
	if (nullptr == Montage)
	{
		return;
	}

	// 몽타주에서 MoveEvent 노티파이를 데이터 마커로 스캔 — 첫 번째 구간 (다중 구간은 추후)
	float StartTime = -1.f;
	float WindowDuration = 0.f;
	float MoveDistance = 0.f;
	for (const FAnimNotifyEvent& Event : Montage->Notifies)
	{
		if (const US1AnimNotifyState_MoveEvent* MoveNotify = Cast<US1AnimNotifyState_MoveEvent>(Event.NotifyStateClass))
		{
			StartTime      = Event.GetTriggerTime();
			WindowDuration = Event.GetDuration();
			MoveDistance   = MoveNotify->GetMoveDistance();
			break;
		}
	}

	// 이동 노티파이 없는 몽타주 = 이동 없음
	if (StartTime < 0.f || WindowDuration <= KINDA_SMALL_NUMBER || FMath::IsNearlyZero(MoveDistance))
	{
		return;
	}

	FVector MoveDir = GetCapturedMoveDirection().GetSafeNormal();
	if (MoveDir.IsNearlyZero())
	{
		if (const AActor* Avatar = GetAvatarActorFromActorInfo())
		{
			MoveDir = Avatar->GetActorForwardVector();
		}
	}

	// 몽타주는 Rate × RateScale 배속으로 재생 → montage-time ≠ real-time
	// 루트모션 task는 real-time 기준이므로 노티파이 montage-time을 real-time으로 변환 (÷ 배속)
	const float EffectiveRate = FMath::Max(Rate * Montage->RateScale, KINDA_SMALL_NUMBER);
	const float RealWindow    = WindowDuration / EffectiveRate;
	const float Speed         = MoveDistance / RealWindow;					// 음수 = 후방
	const float TotalDuration = (StartTime + WindowDuration) / EffectiveRate;
	const float StartNorm     = StartTime / (StartTime + WindowDuration);	// 배속은 분자/분모에서 상쇄 → montage 비율 그대로

	// windup(0~StartNorm) force 0, 노티파이 구간(StartNorm~끝) force 1로 게이트하는 커브
	// → 활성화 시점에 task 생성(예측키 확보)하되 이동은 노티파이 구간에만 정확히 발생
	UCurveFloat* StrengthCurve = nullptr;
	if (StartTime > KINDA_SMALL_NUMBER)
	{
		StrengthCurve = NewObject<UCurveFloat>(this);
		FRichCurve& RichCurve = StrengthCurve->FloatCurve;
		// 명시적 선형 램프: windup(0~StartNorm) 0, 구간 진입에서 즉시 1
		RichCurve.AddKey(0.f, 0.f);
		RichCurve.AddKey(FMath::Max(StartNorm - 0.001f, 0.f), 0.f);
		RichCurve.AddKey(StartNorm, 1.f);
		RichCurve.AddKey(1.f, 1.f);
	}

	if (IsValid(MoveTask))
	{
		MoveTask->EndTask();
		MoveTask = nullptr;
	}

	MoveTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,
		US1AnimNotifyState_MoveEvent::RootMotionSourceName,
		MoveDir,
		Speed,
		TotalDuration,
		false,									// bIsAdditive
		StrengthCurve,							// 구간 게이트 (StartTime==0이면 nullptr=즉시)
		ERootMotionFinishVelocityMode::SetVelocity,
		FVector::ZeroVector,					// SetVelocityOnFinish
		0.f,									// ClampVelocityOnFinish
		bMoveEnableGravity);
	MoveTask->ReadyForActivation();
}

void US1GameplayAbility_Action::OnMoveBeginReceived(const FGameplayEventData* Payload)
{
	// 이동은 PlayAbilityMontage→ApplyMontageRootMotion가 활성화 시점(예측 스코프)에 처리
	// 베이스는 비움 — 서브클래스(Assault 등)가 히트 콜리전 등 비이동 로직만 override
}

void US1GameplayAbility_Action::OnMoveEndReceived(const FGameplayEventData* Payload)
{
	// 루트모션은 유한 Duration으로 스스로 종료(예측키로 동기화) — 여기서 수동 종료 안 함
	// 베이스는 비움 — 서브클래스가 히트 콜리전 off / 분기 등만 처리
}

void US1GameplayAbility_Action::InternalMoveBeginCallback(const FGameplayEventData* Payload)
{
	OnMoveBeginReceived(Payload);
}

void US1GameplayAbility_Action::InternalMoveEndCallback(const FGameplayEventData* Payload)
{
	OnMoveEndReceived(Payload);
}

void US1GameplayAbility_Action::OnEarlyMoveEventReceived(FGameplayEventData Payload)
{
	OnEarlyMoveEnabled();
}

void US1GameplayAbility_Action::OnEarlyMoveEnabled()
{
	// 점프 차단 해제
	if (ActionStateTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->RemoveLooseGameplayTag(ActionStateTag, 1, EGameplayTagReplicationState::TagOnly);
		}
	}

	// 이동/점프 입력 감지 시작 → TriggerInputDetected
	US1AbilityTask_EarlyExitChecker* Task = US1AbilityTask_EarlyExitChecker::Run(this, JumpEventTag);
	Task->OnInputDetected.AddUObject(this, &ThisClass::OnEarlyMoveTriggered);
	Task->ReadyForActivation();
}

void US1GameplayAbility_Action::OnEarlyMoveTriggered()
{
	if (CurrentActorInfo)
	{
		if (UAnimInstance* AnimInst = CurrentActorInfo->GetAnimInstance())
		{
			AnimInst->Montage_Stop(0.15f);
		}
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
