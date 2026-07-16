// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/S1SequenceResource.h"

#include "LevelSequence.h"

ULevelSequence* US1SequenceResource::FindSequenceByTag(const FGameplayTag& SequenceTag) const
{
	for (const FS1Sequence& Entry : Sequences)
	{
		if (Entry.Sequence && Entry.SequenceTag == SequenceTag)
		{
			return Entry.Sequence;
		}
	}

	UE_LOG(LogWindows, Error, TEXT("Can't find Sequence for SequenceTag [%s]"), *SequenceTag.ToString());

	return nullptr;
}