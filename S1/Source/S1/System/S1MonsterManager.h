// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "S1MonsterManager.generated.h"

class AS1Monster;

// 레벨 내 활성 몬스터 목록 — AS1Monster::BeginPlay/EndPlay에서 등록/해제
UCLASS()
class S1_API US1MonsterManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterMonster(AS1Monster* Monster);
	void UnregisterMonster(AS1Monster* Monster);

	const TArray<TWeakObjectPtr<AS1Monster>>& GetActiveMonsters() const { return ActiveMonsters; }

private:
	TArray<TWeakObjectPtr<AS1Monster>> ActiveMonsters;
};
