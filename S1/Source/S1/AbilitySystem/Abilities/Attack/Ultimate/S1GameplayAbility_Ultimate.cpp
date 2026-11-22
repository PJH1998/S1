#include "AbilitySystem/Abilities/Attack/Ultimate/S1GameplayAbility_Ultimate.h"
#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "Character/S1Monster.h"
#include "Character/Player/S1Player.h"
#include "System/S1MonsterManager.h"
#include "LevelSequence.h"
#include "MovieScene.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"

US1GameplayAbility_Ultimate::US1GameplayAbility_Ultimate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 컷씬 연출/몬스터 프리즈는 서버 권위지만 몽타주는 소유 클라에서도 재생돼야 한다 —
	// 그래야 엔진 복제 경로(ASC->PlayMontage)를 타고 섹션 점프도 가능해진다(ServerOnly는 조용히 거부됨).
	// Attack 베이스의 LocalPredicted는 예측할 근거(클라 입력 기반 이동)가 없어 여기서 ServerInitiated로 내림.
	// 아래 서버 권위 동작들은 HasAuthority 가드로 분리한다
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void US1GameplayAbility_Ultimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	bCutsceneActive = false;

	// 발동 즉시 게이지 소모(0으로 리셋) — State.CanUltimate 제거로 다음 Z 입력은 다시 Skill04로 라우팅됨.
	// 어트리뷰트는 서버 권위 — 클라에서 쓰면 복제로 되돌려지며 깜빡인다
	if (UAbilitySystemComponent* ASC = HasAuthority(&ActivationInfo) ? GetAbilitySystemComponentFromActorInfo() : nullptr)
	{
		if (US1PlayerSet* PlayerSet = const_cast<US1PlayerSet*>(Cast<US1PlayerSet>(ASC->GetAttributeSet(US1PlayerSet::StaticClass()))))
		{
			PlayerSet->ResetUltimateGauge();
		}
	}

	// 몽타주(Progression)는 서버에서 재생 → 모든 클라 복제
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (false == ::IsValid(Character))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (false == ::IsValid(CutsceneSequence))
	{
		return;
	}

	UWorld* World = Character->GetWorld();
	if (nullptr == World)
	{
		return;
	}

	// 여기부터는 서버 권위 동작 — ServerInitiated라 이 함수가 소유 클라에서도 돈다.
	// 클라가 Client RPC를 부르면 경고 없이 로컬 실행되어(FunctionCallspace::Local) 컷씬이 이중 재생되고,
	// 타이머까지 돌면 서버의 ClientEndAbility와 종료가 경쟁한다
	if (false == HasAuthority(&ActivationInfo))
	{
		return;
	}

	// 몬스터 프리즈 — 서버 권위 (전체)
	FreezeMonsters(World);

	// 컷씬 비주얼(시퀀스+카메라)은 시전 클라에서만 로컬 재생
	if (AS1Player* Player = Cast<AS1Player>(Character))
	{
		Player->ClientPlayUltimateCutscene(CutsceneSequence, PlayerBindingTag, EnterBlendTime, ExitBlendTime);
	}

	// 서버: 시퀀스 길이 동안 프리즈 유지 후 종료 (비주얼 타이밍과 동일 소스라 대략 동기)
	const float DurationSec = ComputeSequenceDurationSeconds();
	bCutsceneActive = true;
	World->GetTimerManager().SetTimer(
		CutsceneServerTimer,
		FTimerDelegate::CreateWeakLambda(this, [this]() { OnServerCutsceneComplete(); }),
		FMath::Max(0.1f, DurationSec),
		false);
}

void US1GameplayAbility_Ultimate::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 컷씬 진행 중 몽타주가 먼저 끝나도 defer — 서버 타이머(OnServerCutsceneComplete)가 종료.
	// ⚠️ 서버에서만. 클라는 bCutsceneActive를 지울 타이머가 없어서 defer하면 어빌리티가 영영 안 끝난다
	// (서버가 보내는 ClientEndAbility를 그대로 따르게 둔다)
	if (HasAuthority(&ActivationInfo) && false == bWasCancelled && bCutsceneActive)
	{
		return;
	}

	DoEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_Ultimate::DoEndAbility(const FGameplayAbilitySpecHandle& Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bCutsceneActive = false;

	UnfreezeMonsters();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CutsceneServerTimer);
	}

	// 취소 시: 시전 클라 컷씬 즉시 중단 + 카메라 복귀
	if (bWasCancelled)
	{
		if (AS1Player* Player = HasAuthority(&ActivationInfo) ? Cast<AS1Player>(GetAvatarActorFromActorInfo()) : nullptr)
		{
			Player->ClientStopUltimateCutscene();
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_Ultimate::OnServerCutsceneComplete()
{
	bCutsceneActive = false;
	UnfreezeMonsters();
	DoEndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

float US1GameplayAbility_Ultimate::ComputeSequenceDurationSeconds() const
{
	if (false == ::IsValid(CutsceneSequence))
	{
		return 0.f;
	}

	UMovieScene* MovieScene = CutsceneSequence->GetMovieScene();
	if (nullptr == MovieScene)
	{
		return 0.f;
	}

	const FFrameRate TickResolution = MovieScene->GetTickResolution();
	const TRange<FFrameNumber> Range = MovieScene->GetPlaybackRange();
	const FFrameNumber FrameCount = Range.Size<FFrameNumber>();

	return static_cast<float>(TickResolution.AsSeconds(FFrameTime(FrameCount)));
}

void US1GameplayAbility_Ultimate::FreezeMonsters(UWorld* World)
{
	US1MonsterManager* MonsterManager = World->GetSubsystem<US1MonsterManager>();
	if (false == ::IsValid(MonsterManager))
	{
		return;
	}

	for (const TWeakObjectPtr<AS1Monster>& WeakMonster : MonsterManager->GetActiveMonsters())
	{
		if (AS1Monster* Monster = WeakMonster.Get())
		{
			// 복제 Multicast — 전체 클라에서 CustomTimeDilation=0 적용 (애니메이션까지 정지)
			Monster->MulticastSetTimeFrozen(true);
			FrozenMonsters.Add(Monster);
		}
	}
}

void US1GameplayAbility_Ultimate::UnfreezeMonsters()
{
	for (const TWeakObjectPtr<AS1Monster>& WeakMonster : FrozenMonsters)
	{
		if (AS1Monster* Monster = WeakMonster.Get())
		{
			Monster->MulticastSetTimeFrozen(false);
		}
	}
	FrozenMonsters.Reset();
}
