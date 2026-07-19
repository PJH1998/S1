// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTagContainer.h"
#include "S1PlayerState.generated.h"


class US1PlayerSet;
class US1AbilitySystemComponent;
class US1InventoryComponent;
class US1EquipComponent;

UCLASS()
class S1_API AS1PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AS1PlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	US1AbilitySystemComponent* GetS1AbilitySystemComponent() const;
	US1PlayerSet* GetS1PlayerSet() const;
	US1InventoryComponent* GetInventoryComponent() const;
	US1EquipComponent* GetEquipComponent() const;
	void InitPlayerSetFromTable(const FGameplayTag& AssetTag, const FGameplayTag& TableTag);

	void SetSelectedCharacterTag(const FGameplayTag& InTag);
	FGameplayTag GetSelectedCharacterTag() const { return SelectedCharacterTag; }

	void SetLastSpawnPointTag(const FGameplayTag& InTag);
	FGameplayTag GetLastSpawnPointTag() const { return LastSpawnPointTag; }

	void SetSelectedWeaponTag(const FGameplayTag& InTag);
	FGameplayTag GetSelectedWeaponTag() const { return SelectedWeaponTag; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Seamless Travel: 구 PS → 신 PS 복사
	virtual void CopyProperties(APlayerState* PlayerState) override;
	// Non-Seamless (재접속): inactive 상태에서 신 PS 로 복원
	virtual void OverrideWith(APlayerState* PlayerState) override;

private:
	UPROPERTY(Replicated)
	FGameplayTag SelectedCharacterTag;

	UPROPERTY(Replicated)
	FGameplayTag LastSpawnPointTag;

	UPROPERTY(Replicated)
	FGameplayTag SelectedWeaponTag;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1AbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<US1PlayerSet> PlayerSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1InventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1EquipComponent> EquipComponent;
};
