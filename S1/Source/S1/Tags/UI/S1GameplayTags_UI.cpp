// Fill out your copyright notice in the Description page of Project Settings.

#include "Tags/UI/S1GameplayTags_UI.h"

namespace S1UITags
{
	UE_DEFINE_GAMEPLAY_TAG(UI_Root_Gameplay, "UI.Root.Gameplay");
	UE_DEFINE_GAMEPLAY_TAG(UI_Root_Lobby, "UI.Root.Lobby");
	UE_DEFINE_GAMEPLAY_TAG(UI_Cursor, "UI.Cursor");
	UE_DEFINE_GAMEPLAY_TAG(UI_Fade, "UI.Fade");

	UE_DEFINE_GAMEPLAY_TAG(UI_Lobby_HUD, "UI.Lobby.HUD");
	UE_DEFINE_GAMEPLAY_TAG(UI_Lobby_Logo, "UI.Lobby.Logo");
	UE_DEFINE_GAMEPLAY_TAG(UI_Gameplay_HUD, "UI.Gameplay.HUD");
	UE_DEFINE_GAMEPLAY_TAG(UI_Monster_HPBar, "UI.Monster.HPBar");
	UE_DEFINE_GAMEPLAY_TAG(UI_Gameplay_DamageNumber, "UI.Gameplay.DamageNumber");

	UE_DEFINE_GAMEPLAY_TAG(UI_Menu_Inventory, "UI.Menu.Inventory");
	UE_DEFINE_GAMEPLAY_TAG(UI_Menu_ItemInfo, "UI.Menu.ItemInfo");

	UE_DEFINE_GAMEPLAY_TAG(UI_Cooldown_Skill01, "UI.Cooldown.Skill01");
}

namespace S1UIResourceTags
{
	UE_DEFINE_GAMEPLAY_TAG(UI_Tex_Heal, "UI.Tex.Heal");

	UE_DEFINE_GAMEPLAY_TAG(UI_Tex_Asna_Skill01, "UI.Tex.Asna.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(UI_Tex_Asna_Skill02, "UI.Tex.Asna.Skill02");
	UE_DEFINE_GAMEPLAY_TAG(UI_Tex_Asna_Skill03, "UI.Tex.Asna.Skill03");

#pragma region Inventory
	UE_DEFINE_GAMEPLAY_TAG(UI_Icon_BG_Common, "UI.Icon.BG.Common");
	UE_DEFINE_GAMEPLAY_TAG(UI_Icon_BG_Rare, "UI.Icon.BG.Rare");
	UE_DEFINE_GAMEPLAY_TAG(UI_Icon_BG_Unique, "UI.Icon.BG.Unique");
	UE_DEFINE_GAMEPLAY_TAG(UI_Icon_BG_Legendary, "UI.Icon.BG.Legendary");
	UE_DEFINE_GAMEPLAY_TAG(UI_Icon_Frame_Common, "UI.Icon.Frame.Common");
	UE_DEFINE_GAMEPLAY_TAG(UI_Icon_Frame_Rare, "UI.Icon.Frame.Rare");
	UE_DEFINE_GAMEPLAY_TAG(UI_Icon_Frame_Unique, "UI.Icon.Frame.Unique");
	UE_DEFINE_GAMEPLAY_TAG(UI_Icon_Frame_Legendary, "UI.Icon.Frame.Legendary");
	UE_DEFINE_GAMEPLAY_TAG(UI_Icon_Effect_Equiped, "UI.Icon.Effect.Equiped");
#pragma endregion

#pragma region Select
	UE_DEFINE_GAMEPLAY_TAG(UI_Icon_Weapon_RPR, "UI.Icon.Weapon.RPR");
	UE_DEFINE_GAMEPLAY_TAG(UI_Icon_Weapon_SWD, "UI.Icon.Weapon.SWD");
	UE_DEFINE_GAMEPLAY_TAG(UI_Icon_Weapon_DSWD, "UI.Icon.Weapon.DSWD");
#pragma endregion
}
