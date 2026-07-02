// Fill out your copyright notice in the Description page of Project Settings.


#include "S1Menu_SelectCharacter.h"
#include "UI/Lobby/Custom/Select/S1SelectCharacter_Slot.h"
#include "UI/Lobby/Custom/Select/S1SelectWeapon_Slot.h"
#include "Components/TextBlock.h"
#include "Player/S1CharacterSelectController.h"
#include "Tags/S1GameplayTags.h"

void US1Menu_SelectCharacter::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectCharSlot_M)
	{
		SelectCharSlot_M->OnSlotClicked.AddDynamic(this, &ThisClass::OnSlotMClicked);
	}

	if (SelectCharSlot_F)
	{
		SelectCharSlot_F->OnSlotClicked.AddDynamic(this, &ThisClass::OnSlotFClicked);
	}

	if (SelectWeapon_01)
	{
		SelectWeapon_01->OnSlotClicked.AddDynamic(this, &ThisClass::OnWeaponSlot01Clicked);
	}

	if (SelectWeapon_02)
	{
		SelectWeapon_02->OnSlotClicked.AddDynamic(this, &ThisClass::OnWeaponSlot02Clicked);
	}

	// 컨트롤러의 기본 선택(BeginPlay에서 CharacterTagA = Asuna)과 초기 표시를 맞춘다.
	SelectSlot(SelectCharSlot_F, SelectCharSlot_M, FText::FromString(TEXT("아스나")));
	SetupWeaponSlots(S1ItemTags::Item_Weapon_RPR00, S1ItemTags::Item_Weapon_SWD00);
}

void US1Menu_SelectCharacter::OnSlotMClicked()
{
	SelectSlot(SelectCharSlot_M, SelectCharSlot_F, FText::FromString(TEXT("키리토")));
	SetupWeaponSlots(S1ItemTags::Item_Weapon_DSWD01, S1ItemTags::Item_Weapon_SWD00);

	if (AS1CharacterSelectController* Controller = GetOwningPlayer<AS1CharacterSelectController>())
	{
		Controller->OnSelectB();
	}
}

void US1Menu_SelectCharacter::OnSlotFClicked()
{
	SelectSlot(SelectCharSlot_F, SelectCharSlot_M, FText::FromString(TEXT("아스나")));
	SetupWeaponSlots(S1ItemTags::Item_Weapon_RPR00, S1ItemTags::Item_Weapon_SWD00);

	if (AS1CharacterSelectController* Controller = GetOwningPlayer<AS1CharacterSelectController>())
	{
		Controller->OnSelectA();
	}
}

void US1Menu_SelectCharacter::OnWeaponSlot01Clicked()
{
	SelectWeaponSlot(SelectWeapon_01, SelectWeapon_02);

	if (AS1CharacterSelectController* Controller = GetOwningPlayer<AS1CharacterSelectController>())
	{
		Controller->OnWeaponSelect(SelectWeapon_01->GetWeaponTag());
	}
}

void US1Menu_SelectCharacter::OnWeaponSlot02Clicked()
{
	SelectWeaponSlot(SelectWeapon_02, SelectWeapon_01);

	if (AS1CharacterSelectController* Controller = GetOwningPlayer<AS1CharacterSelectController>())
	{
		Controller->OnWeaponSelect(SelectWeapon_02->GetWeaponTag());
	}
}

void US1Menu_SelectCharacter::SelectSlot(US1SelectCharacter_Slot* SelectedSlot, US1SelectCharacter_Slot* OtherSlot, const FText& InNameplateText)
{
	if (SelectedSlot)
	{
		SelectedSlot->SetSelected(true);
	}

	if (OtherSlot)
	{
		OtherSlot->SetSelected(false);
	}

	if (Text_Nameplate)
	{
		Text_Nameplate->SetText(InNameplateText);
	}
}

void US1Menu_SelectCharacter::SelectWeaponSlot(US1SelectWeapon_Slot* SelectedSlot, US1SelectWeapon_Slot* OtherSlot)
{
	if (SelectedSlot)
	{
		SelectedSlot->SetSelected(true);
	}

	if (OtherSlot)
	{
		OtherSlot->SetSelected(false);
	}
}

void US1Menu_SelectCharacter::SetupWeaponSlots(const FGameplayTag& Slot01WeaponTag, const FGameplayTag& Slot02WeaponTag)
{
	if (SelectWeapon_01)
	{
		SelectWeapon_01->SetWeaponTag(Slot01WeaponTag);
	}

	if (SelectWeapon_02)
	{
		SelectWeapon_02->SetWeaponTag(Slot02WeaponTag);
	}

	SelectWeaponSlot(SelectWeapon_01, SelectWeapon_02);
}

