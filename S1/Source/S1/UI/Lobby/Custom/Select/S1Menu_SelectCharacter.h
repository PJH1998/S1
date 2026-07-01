// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/S1BaseWidget.h"
#include "S1Menu_SelectCharacter.generated.h"

class US1SelectCharacter_Slot;
class US1SelectWeapon_Slot;
class UTextBlock;

/**
 *
 */
UCLASS()
class S1_API US1Menu_SelectCharacter : public US1BaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnSlotMClicked();

	UFUNCTION()
	void OnSlotFClicked();

	UFUNCTION()
	void OnWeaponSlot01Clicked();

	UFUNCTION()
	void OnWeaponSlot02Clicked();

	void SelectSlot(US1SelectCharacter_Slot* SelectedSlot, US1SelectCharacter_Slot* OtherSlot, const FText& InNameplateText);
	void SelectWeaponSlot(US1SelectWeapon_Slot* SelectedSlot, US1SelectWeapon_Slot* OtherSlot);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1SelectCharacter_Slot> SelectCharSlot_M;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1SelectCharacter_Slot> SelectCharSlot_F;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1SelectWeapon_Slot> SelectWeapon_01;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<US1SelectWeapon_Slot> SelectWeapon_02;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Nameplate;
};
