#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1WeaponData.generated.h"

class AS1Weapon;
class US1WeaponAnimLayer;

USTRUCT(BlueprintType)
struct FS1WeaponEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AS1Weapon> WeaponClass;

	// null이면 단일 무기, 설정 시 오프핸드 소켓에 추가 스폰
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AS1Weapon> OffhandWeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<US1WeaponAnimLayer> MaleAnimLayerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<US1WeaponAnimLayer> FemaleAnimLayerClass;

	// 이 무기 장착 시 부여할 GA 그룹 태그 (AbilityData PDA 키)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag WeaponAbilitiesTag;
};

UCLASS()
class S1_API US1WeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	const FS1WeaponEntry* FindEntryByTag(const FGameplayTag& ItemTag) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, FS1WeaponEntry> WeaponEntries;
};
