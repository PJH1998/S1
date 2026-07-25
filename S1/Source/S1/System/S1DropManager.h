// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Monster/S1DropItemResource.h"
#include "Data/Monster/S1MonsterDropTable.h"
#include "Subsystems/WorldSubsystem.h"
#include "S1DropManager.generated.h"

class AController;
class AS1Monster;

UCLASS()
class S1_API US1DropManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	void HandleMonsterDeath(AS1Monster* DeadMonster);
	void HandleMonsterDeathForOwners(AS1Monster* DeadMonster, const TArray<AController*>& RewardOwners);

	// 몬스터와 무관하게, 임의의 DropTableTag를 지정한 대상들에게 지급(퍼즐 완료 보상 등 레벨/BP에서 직접 바인딩).
	UFUNCTION(BlueprintCallable, Category = "Drop")
	void GrantDropTableRewards(FGameplayTag DropTableTag, FVector OriginLocation, const TArray<AController*>& RewardOwners);

	// 위와 동일하되, 대상을 월드에 접속한 모든 PlayerController로 자동 수집(몬스터 처치 보상과 동일한 대상 범위).
	UFUNCTION(BlueprintCallable, Category = "Drop")
	void GrantDropTableRewardsToAllPlayers(FGameplayTag DropTableTag, FVector OriginLocation);

private:
	void SpawnDropsForOwner(const US1MonsterDropTable& DropTable, FVector OriginLocation, AController* OwnerController);
	void SpawnDropItem(ES1DropItemType DropType, int32 Amount, FGameplayTag ItemTag, FGameplayTag RarityTag, FVector OriginLocation, AController* OwnerController);

	// 무기 아이템 드랍이면 획득자가 장착한 무기 계열의 같은 등급 무기로 치환 — 비무기이거나 해석 실패 시 원본 태그 반환
	FGameplayTag ResolveWeaponItemTagForOwner(FGameplayTag ItemTag, FGameplayTag RarityTag, AController* OwnerController) const;

	const FS1DropItemEntry* SelectDropItem(const FS1DropPool& DropPool) const;
	bool ShouldDropPool(const FS1DropPool& DropPool) const;
};
