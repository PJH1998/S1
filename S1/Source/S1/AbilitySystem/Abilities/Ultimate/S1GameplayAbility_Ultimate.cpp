// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Ultimate/S1GameplayAbility_Ultimate.h"
#include "Character/S1Monster.h"
#include "System/S1MonsterManager.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "MovieSceneTimeController.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"

US1GameplayAbility_Ultimate::US1GameplayAbility_Ultimate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1GameplayAbility_Ultimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
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
	UnfreezeMonsters();

	if (::IsValid(SequencePlayer))
	{
		SequencePlayer->OnFinished.RemoveAll(this);
		SequencePlayer->Stop();
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

	// Sequencer의 "PlayerAnchor" Tag 바인딩을 스폰 대신 실제 플레이어로 대체
	// (스폰/바인딩 해석 전에 호출해야 함 — Play() 이전)
	SequenceActor->SetBindingByTag(PlayerBindingTag, { Character });

	SequencePlayer->OnFinished.AddDynamic(this, &ThisClass::OnCutsceneFinished);

	SequencePlayer->Play();
}

void US1GameplayAbility_Ultimate::OnCutsceneFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
