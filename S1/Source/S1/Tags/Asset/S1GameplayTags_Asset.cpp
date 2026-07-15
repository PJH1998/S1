// Fill out your copyright notice in the Description page of Project Settings.

#include "Tags/Asset/S1GameplayTags_Asset.h"

namespace S1Labels
{
	UE_DEFINE_GAMEPLAY_TAG(Label_Preload, "Label.Preload");
}

namespace S1AssetTags
{
#pragma region Input
	UE_DEFINE_GAMEPLAY_TAG(Asset_InputData, "Asset.InputData");
#pragma endregion

#pragma region UI
	UE_DEFINE_GAMEPLAY_TAG(Asset_UIData,     "Asset.UIData");
	UE_DEFINE_GAMEPLAY_TAG(Asset_UIResource, "Asset.UIResource");
#pragma endregion

#pragma region GameAbilitySystem
	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability_Player_Default, "Asset.Ability.Player.Default");

	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability_Player_RPR_Female, "Asset.Ability.Player.RPR.Female");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability_Player_SWD_Female, "Asset.Ability.Player.SWD.Female");

	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability_Player_SWD_Male, "Asset.Ability.Player.SWD.Male");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability_Player_DSWD_Male, "Asset.Ability.Player.DSWD.Male");

	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability_Boss000,  "Asset.Ability.Boss000");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability_Enemy001, "Asset.Ability.Enemy001");

	UE_DEFINE_GAMEPLAY_TAG(Asset_GameplayEffect, "Asset.GameplayEffect");
#pragma endregion

#pragma region Animation
	UE_DEFINE_GAMEPLAY_TAG(Asset_AnimData,             "Asset.AnimData");
#pragma endregion

#pragma region Effect
	UE_DEFINE_GAMEPLAY_TAG(Asset_Effect, "Asset.Effect");
#pragma endregion

#pragma region Sound
	UE_DEFINE_GAMEPLAY_TAG(Asset_SoundData, "Asset.SoundData");
#pragma endregion

#pragma region Datas
	UE_DEFINE_GAMEPLAY_TAG(Asset_PoolingData, "Asset.PoolingData");
	UE_DEFINE_GAMEPLAY_TAG(Asset_QuestData, "Asset.QuestData");

	UE_DEFINE_GAMEPLAY_TAG(Asset_DataTable, "Asset.DataTable");

	UE_DEFINE_GAMEPLAY_TAG(Asset_DropItemResource,   "Asset.DropItemResource");
	UE_DEFINE_GAMEPLAY_TAG(Asset_DropTable_Boss000,  "Asset.DropTable.Boss000");
	UE_DEFINE_GAMEPLAY_TAG(Asset_DropTable_Enemy001, "Asset.DropTable.Enemy001");

	UE_DEFINE_GAMEPLAY_TAG(Asset_DropTable_Puzzle, "Asset.DropTable.Puzzle");

	UE_DEFINE_GAMEPLAY_TAG(Asset_WeaponData, "Asset.WeaponData");

	UE_DEFINE_GAMEPLAY_TAG(Asset_CharacterData, "Asset.CharacterData");
#pragma endregion
}

namespace S1DataTableTags
{
	UE_DEFINE_GAMEPLAY_TAG(DataTable_PlayerData,  "DataTable.PlayerData");
	UE_DEFINE_GAMEPLAY_TAG(DataTable_MonsterData, "DataTable.MonsterData");
	UE_DEFINE_GAMEPLAY_TAG(DataTable_ItemData,    "DataTable.ItemData");
	UE_DEFINE_GAMEPLAY_TAG(DataTable_HitLagData,  "DataTable.HitLagData");
	UE_DEFINE_GAMEPLAY_TAG(DataTable_FootstepSound, "DataTable.FootstepSound");
}
