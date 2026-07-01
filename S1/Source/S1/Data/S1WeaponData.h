#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
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

	// 이 무기 장착 시 부여할 GA 그룹 태그 (AbilityData PDA 키)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag WeaponAbilitiesTag;

	// 이 장착 조합 전용 AnimLayer — WeaponClass 단위가 아닌 아이템 단위로 결정
	// (쌍검은 한손검 WeaponClass를 Main/Offhand에 재사용하므로 AnimLayer를 WeaponClass에 둘 수 없음)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<US1WeaponAnimLayer> MaleAnimLayerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<US1WeaponAnimLayer> FemaleAnimLayerClass;

	TSubclassOf<US1WeaponAnimLayer> GetAnimLayerClass(EPlayerGender InGender) const
	{
		return (InGender == EPlayerGender::Female) ? FemaleAnimLayerClass : MaleAnimLayerClass;
	}
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
