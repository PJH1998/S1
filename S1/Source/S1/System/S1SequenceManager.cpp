// Fill out your copyright notice in the Description page of Project Settings.

#include "System/S1SequenceManager.h"

#include "Data/S1SequenceResource.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "System/S1AssetManager.h"
#include "Tags/Asset/S1GameplayTags_Asset.h"

ULevelSequencePlayer* US1SequenceManager::PrepareSequence(FGameplayTag SequenceTag)
{
	if (TObjectPtr<ULevelSequencePlayer>* Existing = CachedPlayers.Find(SequenceTag))
	{
		if (::IsValid(*Existing))
		{
			return *Existing;
		}
	}

	const US1SequenceResource* SequenceResource = US1AssetManager::GetAssetByTag<US1SequenceResource>(S1AssetTags::Asset_SequenceResource);
	if (false == ::IsValid(SequenceResource))
	{
		return nullptr;
	}

	ULevelSequence* Sequence = SequenceResource->FindSequenceByTag(SequenceTag);
	if (false == ::IsValid(Sequence))
	{
		return nullptr;
	}

	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bAutoPlay = false;

	ALevelSequenceActor* OutActor = nullptr;
	ULevelSequencePlayer* Player = ULevelSequencePlayer::CreateLevelSequencePlayer(this, Sequence, Settings, OutActor);
	CachedPlayers.Add(SequenceTag, Player);

	return Player;
}

ULevelSequencePlayer* US1SequenceManager::PlaySequenceByTag(FGameplayTag SequenceTag)
{
	TObjectPtr<ULevelSequencePlayer>* Found = CachedPlayers.Find(SequenceTag);
	if (nullptr == Found || false == ::IsValid(*Found))
	{
		return nullptr;
	}

	(*Found)->Play();

	return *Found;
}

void US1SequenceManager::StopSequenceByTag(FGameplayTag SequenceTag)
{
	TObjectPtr<ULevelSequencePlayer>* Found = CachedPlayers.Find(SequenceTag);
	if (nullptr == Found || false == ::IsValid(*Found))
	{
		return;
	}

	(*Found)->Stop();
}