// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
#include "S1CharacterSelectData.generated.h"

class AS1Player;
class USkeletalMesh;

USTRUCT(BlueprintType)
struct FS1SelectCharacterEntry
{
	GENERATED_BODY()

	// seamless travel 시 스폰할 플레이어 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AS1Player> CharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EPlayerGender Gender = EPlayerGender::Female;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkeletalMesh> BodyMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkeletalMesh> HairMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkeletalMesh> FaceMesh;

	// WeaponData에서 조회할 시작 무기 태그 (FS1WeaponEntry 키)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag DefaultWeaponTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;
};

UCLASS()
class S1_API US1CharacterSelectData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	const FS1SelectCharacterEntry* FindEntryByTag(const FGameplayTag& Tag) const;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, FS1SelectCharacterEntry> Entries;
};
