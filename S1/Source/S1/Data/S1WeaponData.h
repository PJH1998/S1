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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<US1WeaponAnimLayer> AnimLayerClass;
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
