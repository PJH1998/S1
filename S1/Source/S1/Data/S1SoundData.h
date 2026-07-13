// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1SoundData.generated.h"

class USoundBase;

// 사운드 태그 조회용 데이터 에셋 — Hit Sound뿐 아니라 앞으로 추가될 다른 태그 기반 사운드도 여기 모은다.
// HitSounds Key = BaseTag(무기별 사운드 카테고리, 예: Sound.Hit.RPR)와 OutcomeTag(Weak/Strong/ToAir 등)를
// 조합해 미리 선언해둔 태그(예: Sound.Hit.RPR.Strong) — 조합 로직은 US1SoundManager::PlayHitSound 참조.
UCLASS()
class S1_API US1SoundData : public UDataAsset
{
	GENERATED_BODY()

public:
	USoundBase* FindSound(const FGameplayTag& SoundTag) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TMap<FGameplayTag, TObjectPtr<USoundBase>> HitSounds;
};
