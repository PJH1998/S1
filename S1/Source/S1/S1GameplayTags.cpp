// Fill out your copyright notice in the Description page of Project Settings.


#include "S1GameplayTags.h"

namespace S1GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move, "Input.Action.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Sprint, "Input.Action.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Turn, "Input.Action.Turn");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Attack, "Input.Action.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Skill01, "Input.Action.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Skill02, "Input.Action.Skill02");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Skill03, "Input.Action.Skill03");
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

namespace S1UIResourceTags
{
	UE_DEFINE_GAMEPLAY_TAG(UI_Tex_Heal, "UI.Tex.Heal");

	UE_DEFINE_GAMEPLAY_TAG(UI_Tex_Asna_Skill01, "UI.Tex.Asna.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(UI_Tex_Asna_Skill02, "UI.Tex.Asna.Skill02");
	UE_DEFINE_GAMEPLAY_TAG(UI_Tex_Asna_Skill03, "UI.Tex.Asna.Skill03");
}

namespace S1AnimTags
{
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_Weak, "Anim.Montage.PRP_Weak");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_Skill01, "Anim.Montage.RPR_Skill01");
}

namespace S1AbilityTags
{
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_WeakAttack, "Ability.Attack.WeakAttack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Skill01, "Ability.Attack.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Skill02, "Ability.Attack.Skill02");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Skill03, "Ability.Attack.Skill03");
}

namespace S1Labels
{
	UE_DEFINE_GAMEPLAY_TAG(Label_Preload, "Label.Preload");
}

namespace S1AssetTags
{
	UE_DEFINE_GAMEPLAY_TAG(Asset_InputData, "Asset.InputData");
	UE_DEFINE_GAMEPLAY_TAG(Asset_UIData, "Asset.UIData");
	UE_DEFINE_GAMEPLAY_TAG(Asset_UIResource, "Asset.UIResource");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability,"Asset.Ability");
	UE_DEFINE_GAMEPLAY_TAG(Asset_AnimData, "Asset.AnimData");
}