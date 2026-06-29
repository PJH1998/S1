#include "AbilitySystem/Abilities/Attack/Ultimate/S1GameplayAbility_Ultimate.h"
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
}

void US1GameplayAbility_Ultimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	bCutsceneActive = false;

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
	// 컷씬 진행 중 몽타주가 먼저 끝나도 defer — 서버 타이머(OnServerCutsceneComplete)가 종료
	if (false == bWasCancelled && bCutsceneActive)
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
		if (AS1Player* Player = Cast<AS1Player>(GetAvatarActorFromActorInfo()))
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
