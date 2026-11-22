// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "Tags/GAS/S1GameplayTags_GAS.h"
#include "AbilitySystem/Task/S1AbilityTask_EarlyExitChecker.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "AbilitySystem/Task/S1AbilityTask_DelayedRootMotionForce.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/S1AnimInstance.h"
#include "Animation/NotifyState/S1AnimNotifyState_MoveEvent.h"
#include "Character/S1Character.h"
#include "Character/Player/S1Player.h"
#include "S1LogChannels.h"

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
	SetActionStateTagHeld(true);

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
	SetActionStateTagHeld(false);

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
	OnAbilityMontagePlayed(Montage, Rate, StartSection);

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

		// 활성화 스코프에서 루트모션 생성 — 몽타주의 MoveEvent 노티파이를 데이터로 읽음
		// 노티파이 콜백에서 만들면 클라(예측 즉시)/서버(RPC 도착 후) 발화 시각이 어긋나 스냅백 (실측, 원인 미상)
		// 루트모션 자체는 예측키를 쓰지 않는다 — 클라/서버 정합은 CMC가 saved move로 한다
		ApplyMontageRootMotion(Montage, Rate, StartSection);

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

void US1GameplayAbility_Action::JumpToMontageSection(UAnimMontage* Montage, FName SectionName, float Rate)
{
	if (nullptr == Montage || SectionName.IsNone())
	{
		return;
	}

	// ServerOnly GA는 MulticastPlayMontage로 재생해 ASC의 LocalAnimMontageInfo가 비어 있다 →
	// 엔진 MontageJumpToSection이 IsAnimatingAbility 가드에서 조용히 무시한다
	if (EGameplayAbilityNetExecutionPolicy::LocalPredicted != GetNetExecutionPolicy())
	{
		LOG_WARNING(TEXT("JumpToMontageSection은 LocalPredicted GA 전용 — 무시됨 (%s)"), *GetName());
		return;
	}

	MontageJumpToSection(SectionName);

	OnAbilityMontagePlayed(Montage, Rate, SectionName);
	ApplyMontageRootMotion(Montage, Rate, SectionName);
}

bool US1GameplayAbility_Action::GetMontageScanRange(const UAnimMontage* Montage, FName SectionName, float& OutStartTime, float& OutEndTime)
{
	if (nullptr == Montage)
	{
		return false;
	}

	OutStartTime = 0.f;
	OutEndTime   = Montage->GetPlayLength();

	if (SectionName.IsNone())
	{
		return true;
	}

	const int32 SectionIndex = Montage->GetSectionIndex(SectionName);
	if (INDEX_NONE == SectionIndex)
	{
		return true;
	}

	Montage->GetSectionStartAndEndTime(SectionIndex, OutStartTime, OutEndTime);
	return true;
}

void US1GameplayAbility_Action::ApplyMontageRootMotion(UAnimMontage* Montage, float Rate, FName SectionName)
{
	// 예측 어빌리티만 — ServerOnly는 notify가 CMC에 직접 주입하는 경로를 쓴다
	if (EGameplayAbilityNetExecutionPolicy::LocalPredicted != GetNetExecutionPolicy())
	{
		return;
	}
	if (nullptr == Montage)
	{
		return;
	}

	float ScanStart = 0.f;
	float ScanEnd   = 0.f;
	if (false == GetMontageScanRange(Montage, SectionName, ScanStart, ScanEnd))
	{
		return;
	}

	// 몽타주에서 MoveEvent 노티파이를 데이터 마커로 스캔 — 구간 내 첫 번째 (다중 구간은 추후)
	float StartTime = -1.f;
	float WindowDuration = 0.f;
	float MoveDistance = 0.f;
	for (const FAnimNotifyEvent& Event : Montage->Notifies)
	{
		if (const US1AnimNotifyState_MoveEvent* MoveNotify = Cast<US1AnimNotifyState_MoveEvent>(Event.NotifyStateClass))
		{
			// 구간에 속한 것만 (시작 시각 기준 — 이전 섹션에서 걸쳐 들어온 건 그 섹션 몫)
			const float TriggerTime = Event.GetTriggerTime();
			if (TriggerTime < ScanStart || TriggerTime >= ScanEnd)
			{
				continue;
			}

			// 재생이 구간 시작에서 출발하므로 구간 시작을 0으로 환산
			StartTime      = TriggerTime - ScanStart;
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
	const float RealWindup    = StartTime / EffectiveRate;
	const float TotalDuration = RealWindup + RealWindow;

	if (IsValid(MoveTask))
	{
		MoveTask->EndTask();
		MoveTask = nullptr;
	}

	// 활성화 시점에 task를 만들되 이동은 노티파이 구간에만 발생시킨다.
	// 게이트를 커브가 아니라 float(초)로 넘기는 이유는 US1AbilityTask_DelayedRootMotionForce 주석 참조
	MoveTask = US1AbilityTask_DelayedRootMotionForce::ApplyDelayedRootMotionConstantForce(
		this,
		US1AnimNotifyState_MoveEvent::RootMotionSourceName,
		MoveDir,
		Speed,
		TotalDuration,
		RealWindup,								// 이 시간 전에는 이동 없음
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
	// 루트모션은 유한 Duration으로 스스로 종료 — 여기서 수동 종료 안 함 (클라/서버 타이밍 desync 방지)
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
	SetActionStateTagHeld(false);

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

void US1GameplayAbility_Action::SetActionStateTagHeld(bool bHeld)
{
	if (bHeld == bActionStateTagHeld || false == ActionStateTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (nullptr == ASC)
	{
		return;
	}

	// LocalPredicted: 소유 클라가 GA를 직접 실행 → 각 머신이 로컬로 관리(복제 X)
	// ServerOnly: 소유 클라는 GA를 안 돌림 → 복제 하위 태그 사용(State_Action_Replicated 선언부 주석 참조)
	const bool bReplicate = EGameplayAbilityNetExecutionPolicy::LocalPredicted != GetNetExecutionPolicy();
	const FGameplayTag Tag = bReplicate ? S1StateTags::State_Action_Replicated : ActionStateTag;
	const EGameplayTagReplicationState RepState = bReplicate ? EGameplayTagReplicationState::TagOnly : EGameplayTagReplicationState::None;

	if (bHeld)
	{
		ASC->AddLooseGameplayTag(Tag, 1, RepState);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(Tag, 1, RepState);
	}

	bActionStateTagHeld = bHeld;
}
