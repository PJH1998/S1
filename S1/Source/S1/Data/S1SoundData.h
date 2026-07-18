// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1SoundData.generated.h"

class USoundBase;

// 사운드 태그 조회용 데이터 에셋 — 태그 기반 사운드 공용 맵(히트/기믹/UI/BGM 전부 여기 모은다).
// 히트 사운드 Key는 BaseTag(무기 카테고리: Sword/Fist/Bite 등)와 OutcomeTag(Weak/Strong/ToAir 등)를
// 조합해 미리 선언해둔 태그(예: Sound.Hit.Sword.Strong) — 조합 로직은 US1SoundManager::PlayHitSound 참조.
UCLASS()
class S1_API US1SoundData : public UDataAsset
{
	GENERATED_BODY()

public:
	USoundBase* FindSound(const FGameplayTag& SoundTag) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TMap<FGameplayTag, TObjectPtr<USoundBase>> Sounds;
};
