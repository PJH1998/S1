#include "AbilitySystem/Abilities/Attack/Ultimate/S1GameplayAbility_Ultimate.h"
#include "Character/S1Monster.h"
#include "System/S1MonsterManager.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "DefaultLevelSequenceInstanceData.h"
#include "MovieSceneTimeController.h"
#include "MovieScene.h"
#include "MovieSceneSpawnable.h"
#include "MovieScenePossessable.h"
#include "MovieSceneSequenceID.h"
#include "CineCameraActor.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

US1GameplayAbility_Ultimate::US1GameplayAbility_Ultimate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1GameplayAbility_Ultimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	bCutsceneActive     = false;
	bCameraBlendRetried = false;
	bExitBlendStarted   = false;

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

	FreezeMonsters(World);
	PlayCutscene();
}

void US1GameplayAbility_Ultimate::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (false == bWasCancelled)
	{
		// 컷씬/복귀 블렌드 진행 중이면 몽타주가 먼저 끝나도 defer
		// (OnBlendOutComplete에서 bCutsceneActive=false 후 DoEndAbility 호출)
		if (bCutsceneActive)
		{
			return;
		}
	}

	DoEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GameplayAbility_Ultimate::DoEndAbility(const FGameplayAbilitySpecHandle& Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bCutsceneActive = false;

	UnfreezeMonsters();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ExitBlendTimerHandle);
		World->GetTimerManager().ClearTimer(BlendOutTimerHandle);
	}

	if (::IsValid(SequencePlayer))
	{
		SequencePlayer->OnFinished.RemoveAll(this);
		if (SequencePlayer->IsPlaying())
		{
			SequencePlayer->Stop();
		}
	}

	// 취소 시: 블렌드 없이 즉시 게임플레이 카메라로 복귀 (카메라 파괴 전)
	if (bWasCancelled)
	{
		ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
		if (::IsValid(Character))
		{
			if (APlayerController* PC = Character->GetController<APlayerController>())
			{
				PC->SetViewTargetWithBlend(Character, 0.f);
			}
		}
	}

	// Spawnable 카메라는 Keep State라 시퀀스 종료 후에도 남음 — 직접 파괴
	if (::IsValid(CineCamera))
	{
		CineCamera->Destroy();
		CineCamera = nullptr;
	}

	if (::IsValid(SequenceActor))
	{
		SequenceActor->Destroy();
		SequenceActor = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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
			Monster->CustomTimeDilation = 0.f;
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
			Monster->CustomTimeDilation = 1.f;
		}
	}
	FrozenMonsters.Reset();
}

void US1GameplayAbility_Ultimate::PlayCutscene()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UWorld* World = Character->GetWorld();

	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bAutoPlay = false;

	ALevelSequenceActor* OutActor = nullptr;
	SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(World, CutsceneSequence, Settings, OutActor);
	SequenceActor = OutActor;
	if (false == ::IsValid(SequencePlayer) || false == ::IsValid(SequenceActor))
	{
		return;
	}

	SequencePlayer->SetTimeController(MakeShared<FMovieSceneTimeController_PlatformClock>());

	// 플레이어 앵커만 주입 (반드시 Play 이전) — 카메라는 시퀀스가 Spawnable로 직접 스폰
	SequenceActor->SetBindingByTag(PlayerBindingTag, { Character });

	UDefaultLevelSequenceInstanceData* InstanceData = NewObject<UDefaultLevelSequenceInstanceData>(SequenceActor);
	InstanceData->TransformOrigin = Character->GetActorTransform();
	SequenceActor->DefaultInstanceData = InstanceData;
	SequenceActor->bOverrideInstanceData = true;

	SequencePlayer->OnFinished.AddDynamic(this, &ThisClass::OnCutsceneFinished);
	SequencePlayer->Play();
	bCutsceneActive = true;

	// 스폰된 카메라를 찾아 진입 블렌드
	TryBlendToCineCamera();

	// 시퀀스 종료 (ExitBlendTime + Buffer) 전에 복귀 블렌드 시작
	// — Spawnable 시네캠이 종료 시 디스폰되므로, 살아있는 동안 블렌드를 끝낸다
	const FQualifiedFrameTime TotalDuration = SequencePlayer->GetDuration();
	const float SequenceSec     = static_cast<float>(TotalDuration.Time.AsDecimal() / TotalDuration.Rate.AsDecimal());
	const float Buffer          = 0.1f;
	const float BlendStartDelay = FMath::Max(0.f, SequenceSec - ExitBlendTime - Buffer);

	if (UWorld* TimerWorld = GetWorld())
	{
		TimerWorld->GetTimerManager().SetTimer(
			ExitBlendTimerHandle,
			FTimerDelegate::CreateWeakLambda(this, [this]() { StartExitBlend(); }),
			FMath::Max(0.01f, BlendStartDelay),
			false);
	}
}

void US1GameplayAbility_Ultimate::TryBlendToCineCamera()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	APlayerController* PC = ::IsValid(Character) ? Character->GetController<APlayerController>() : nullptr;
	if (false == ::IsValid(PC))
	{
		return;
	}

	CineCamera = FindSequenceCineCamera();
	if (false == ::IsValid(CineCamera))
	{
		// 스폰 평가가 다음 틱일 수 있음 — 1회만 재시도
		if (false == bCameraBlendRetried)
		{
			bCameraBlendRetried = true;
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().SetTimerForNextTick(
					FTimerDelegate::CreateWeakLambda(this, [this]() { TryBlendToCineCamera(); }));
			}
		}
		return;
	}

	// 게임플레이 → 컷씬 카메라 진입 블렌드 (엔진 네이티브: loc/rot/fov 일괄 이징)
	PC->SetViewTargetWithBlend(CineCamera, EnterBlendTime, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.f);
}

ACineCameraActor* US1GameplayAbility_Ultimate::FindSequenceCineCamera() const
{
	if (false == ::IsValid(SequencePlayer))
	{
		return nullptr;
	}

	UMovieSceneSequence* Sequence = SequencePlayer->GetSequence();
	if (nullptr == Sequence)
	{
		return nullptr;
	}

	UMovieScene* MovieScene = Sequence->GetMovieScene();
	if (nullptr == MovieScene)
	{
		return nullptr;
	}

	// Spawnable 바인딩 우선 검색
	const int32 SpawnableCount = MovieScene->GetSpawnableCount();
	for (int32 Index = 0; Index < SpawnableCount; ++Index)
	{
		const FMovieSceneSpawnable& Spawnable = MovieScene->GetSpawnable(Index);
		for (const TWeakObjectPtr<>& WeakObject : SequencePlayer->FindBoundObjects(Spawnable.GetGuid(), MovieSceneSequenceID::Root))
		{
			if (ACineCameraActor* Camera = Cast<ACineCameraActor>(WeakObject.Get()))
			{
				return Camera;
			}
		}
	}

	// Possessable 바인딩 폴백 (카메라가 Possessable로 구성된 경우 대비)
	const int32 PossessableCount = MovieScene->GetPossessableCount();
	for (int32 Index = 0; Index < PossessableCount; ++Index)
	{
		const FMovieScenePossessable& Possessable = MovieScene->GetPossessable(Index);
		for (const TWeakObjectPtr<>& WeakObject : SequencePlayer->FindBoundObjects(Possessable.GetGuid(), MovieSceneSequenceID::Root))
		{
			if (ACineCameraActor* Camera = Cast<ACineCameraActor>(WeakObject.Get()))
			{
				return Camera;
			}
		}
	}

	return nullptr;
}

void US1GameplayAbility_Ultimate::StartExitBlend()
{
	// 타이머 + OnFinished 폴백 중복 방지
	if (bExitBlendStarted)
	{
		return;
	}
	bExitBlendStarted = true;

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	APlayerController* PC = ::IsValid(Character) ? Character->GetController<APlayerController>() : nullptr;

	// 카메라가 게임플레이로 돌아오기 시작하는 시점에 몬스터 재개
	UnfreezeMonsters();

	// [진단] 복귀 블렌드 시작 시점 — 시네캠이 아직 살아있어야 정상 (CineValid=1)
	if (GEngine && ::IsValid(PC) && ::IsValid(PC->PlayerCameraManager))
	{
		const FVector CurView = PC->PlayerCameraManager->GetCameraLocation();
		const FVector CineLoc = ::IsValid(CineCamera) ? CineCamera->GetActorLocation() : FVector::ZeroVector;
		GEngine->AddOnScreenDebugMessage(80, 15.f, FColor::Cyan,
			FString::Printf(TEXT("[Ulti][EXITBLEND] CineValid=%d  CurView=%s  CineLoc=%s  Dist=%.1f"),
				(int32)::IsValid(CineCamera), *CurView.ToString(), *CineLoc.ToString(),
				FVector::Dist(CurView, CineLoc)));
	}

	if (::IsValid(PC) && ::IsValid(Character))
	{
		// 컷씬 카메라 → 게임플레이 카메라 복귀 블렌드 (엔진 네이티브: loc/rot/fov)
		// 시네캠이 살아있는 동안 블렌드를 시작 → 디스폰 시점엔 이미 폰 도달 → 점프 없음
		PC->SetViewTargetWithBlend(Character, ExitBlendTime, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.f);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			BlendOutTimerHandle,
			FTimerDelegate::CreateWeakLambda(this, [this]() { OnBlendOutComplete(); }),
			FMath::Max(0.01f, ExitBlendTime),
			false);
	}
}

void US1GameplayAbility_Ultimate::OnCutsceneFinished()
{
	// 타이머 기반 복귀 블렌드가 정상이면 여기 도달 전에 이미 시작됨.
	// 폴백: 시퀀스가 예상보다 짧아 타이머가 못 돈 경우 등 — 지금 시작
	StartExitBlend();
}

void US1GameplayAbility_Ultimate::OnBlendOutComplete()
{
	// [진단] 블렌드 완료 시점 — 뷰가 폰 카메라에 도달했는지
	if (GEngine)
	{
		ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
		APlayerController* PC = ::IsValid(Character) ? Character->GetController<APlayerController>() : nullptr;
		if (::IsValid(PC) && ::IsValid(PC->PlayerCameraManager) && ::IsValid(Character))
		{
			UCameraComponent* PawnCam = Character->FindComponentByClass<UCameraComponent>();
			const FVector CurView = PC->PlayerCameraManager->GetCameraLocation();
			const FVector PawnLoc = ::IsValid(PawnCam) ? PawnCam->GetComponentLocation() : FVector::ZeroVector;
			GEngine->AddOnScreenDebugMessage(82, 15.f, FColor::Green,
				FString::Printf(TEXT("[Ulti][BLENDOUT] CineValid=%d  CurView=%s  PawnCam=%s  Dist=%.1f"),
					(int32)::IsValid(CineCamera), *CurView.ToString(), *PawnLoc.ToString(),
					FVector::Dist(CurView, PawnLoc)));
		}
	}

	bCutsceneActive = false;
	DoEndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
