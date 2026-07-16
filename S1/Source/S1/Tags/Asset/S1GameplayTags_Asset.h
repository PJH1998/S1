// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace S1Labels
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Label_Preload);
}

namespace S1AssetTags
{
#pragma region Input
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_InputData);
#pragma endregion

#pragma region UI
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_UIData);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_UIResource);
#pragma endregion

#pragma region Sequence
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_SequenceResource);
#pragma endregion

#pragma region GameAbilitySystem
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Ability_Player_Default);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Ability_Player_RPR_Female);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Ability_Player_SWD_Female);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Ability_Player_SWD_Male);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Ability_Player_DSWD_Male);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Ability_Boss000);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Ability_Enemy001);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_GameplayEffect);
#pragma endregion

#pragma region Animation
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_AnimData);
#pragma endregion

#pragma region Effect
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Effect);
#pragma endregion

#pragma region Sound
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_SoundData);
#pragma endregion

#pragma region Datas
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_PoolingData);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_QuestData);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_DataTable);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_DropItemResource);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_DropTable_Boss000);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_DropTable_Enemy001);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_DropTable_Puzzle);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_WeaponData);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_CharacterData);
#pragma endregion
}

namespace S1DataTableTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataTable_PlayerData);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataTable_MonsterData);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataTable_ItemData);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataTable_HitLagData);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataTable_FootstepSound);
}
