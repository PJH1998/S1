// Fill out your copyright notice in the Description page of Project Settings.

#include "System/S1GimmickManager.h"

#include "Interaction/Gimmick/S1GimmickObject.h"

void US1GimmickManager::RegisterGimmick(FGameplayTag Tag, AS1GimmickObject* Gimmick)
{
	if (false == Tag.IsValid() || false == ::IsValid(Gimmick))
	{
		return;
	}

	ActiveGimmicks.FindOrAdd(Tag).AddUnique(Gimmick);
}

void US1GimmickManager::UnregisterGimmick(FGameplayTag Tag, AS1GimmickObject* Gimmick)
{
	if (TArray<TWeakObjectPtr<AS1GimmickObject>>* Gimmicks = ActiveGimmicks.Find(Tag))
	{
		Gimmicks->RemoveSwap(Gimmick);
	}
}

AS1GimmickObject* US1GimmickManager::GetRandomUsableGimmick(FGameplayTag Tag, AS1GimmickObject* Exclude) const
{
	const TArray<TWeakObjectPtr<AS1GimmickObject>>* Gimmicks = ActiveGimmicks.Find(Tag);
	if (Gimmicks == nullptr)
	{
		return nullptr;
	}

	TArray<AS1GimmickObject*> Candidates;
	for (const TWeakObjectPtr<AS1GimmickObject>& WeakGimmick : *Gimmicks)
	{
		AS1GimmickObject* Gimmick = WeakGimmick.Get();
		if (Gimmick && Gimmick != Exclude && Gimmick->IsUsable())
		{
			Candidates.Add(Gimmick);
		}
	}

	if (Candidates.Num() == 0)
	{
		return nullptr;
	}

	return Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
}

AS1GimmickObject* US1GimmickManager::GetNearestUsableGimmick(FGameplayTag Tag, const FVector& From) const
{
	const TArray<TWeakObjectPtr<AS1GimmickObject>>* Gimmicks = ActiveGimmicks.Find(Tag);
	if (Gimmicks == nullptr)
	{
		return nullptr;
	}

	AS1GimmickObject* Nearest = nullptr;
	float NearestDistSq = TNumericLimits<float>::Max();
	for (const TWeakObjectPtr<AS1GimmickObject>& WeakGimmick : *Gimmicks)
	{
		AS1GimmickObject* Gimmick = WeakGimmick.Get();
		if (nullptr == Gimmick || false == Gimmick->IsUsable())
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(From, Gimmick->GetActorLocation());
		if (DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			Nearest = Gimmick;
		}
	}

	return Nearest;
}

void US1GimmickManager::ResetGimmicks(FGameplayTag Tag)
{
	const TArray<TWeakObjectPtr<AS1GimmickObject>>* Gimmicks = ActiveGimmicks.Find(Tag);
	if (Gimmicks == nullptr)
	{
		return;
	}

	for (const TWeakObjectPtr<AS1GimmickObject>& WeakGimmick : *Gimmicks)
	{
		if (AS1GimmickObject* Gimmick = WeakGimmick.Get())
		{
			Gimmick->ResetGimmick();
		}
	}
}

void US1GimmickManager::ActivateGimmick(FGameplayTag Tag)
{
	if (AS1GimmickObject* Gimmick = GetRandomUsableGimmick(Tag))
	{
		Gimmick->ActivateGimmick();
	}
}
