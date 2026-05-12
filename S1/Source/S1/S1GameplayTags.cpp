// Fill out your copyright notice in the Description page of Project Settings.


#include "S1GameplayTags.h"

namespace S1GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move, "Input.Action.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Turn, "Input.Action.Turn");
}

namespace S1UITags
{
	UE_DEFINE_GAMEPLAY_TAG(UI_Root, "UI.Root");
	UE_DEFINE_GAMEPLAY_TAG(UI_Cursor, "UI.Cursor");
	UE_DEFINE_GAMEPLAY_TAG(UI_Fade, "UI.Fade");

	UE_DEFINE_GAMEPLAY_TAG(UI_Lobby_HUD, "UI.Lobby.HUD");
	UE_DEFINE_GAMEPLAY_TAG(UI_Lobby_Logo, "UI.Lobby.Logo");

	UE_DEFINE_GAMEPLAY_TAG(UI_Gameplay_HUD, "UI.Gameplay.HUD");
}

namespace S1AbilityTags
{
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack, "Ability.Attack");
}

namespace S1Labels
{
	UE_DEFINE_GAMEPLAY_TAG(Label_Preload, "Label.Preload");
}

namespace S1AssetTags
{
	UE_DEFINE_GAMEPLAY_TAG(Asset_InputData, "Asset.InputData");
	UE_DEFINE_GAMEPLAY_TAG(Asset_UIData, "Asset.UIData");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability,"Asset.Ability");
}