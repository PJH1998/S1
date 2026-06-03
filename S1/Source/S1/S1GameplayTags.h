// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace S1GameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Sprint);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Turn);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Jump);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Skill01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Skill02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Skill03);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_LockOn);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_ChangeLockOnSide);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_LockOnLeft);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_LockOnRight);
}

namespace S1UITags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Root);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Cursor);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Fade);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Lobby_HUD);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Lobby_Logo);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Gameplay_HUD);
}

namespace S1AnimTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_RPR_Weak);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_RPR_AirWeak);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_RPR_AirWeakDive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_RPR_Skill01);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_Boss000_Atack00);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_Boss000_Atack01);
}

namespace S1UIResourceTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Tex_Heal);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Tex_Asna_Skill01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Tex_Asna_Skill02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Tex_Asna_Skill03);
}

namespace S1AbilityTags
{
#pragma region Player
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Ground);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Air);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_WeakAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_WeakAttackDive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Skill01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Skill02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Skill03);
#pragma endregion

#pragma region Boss000
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_Attack00);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_Attack01);
#pragma endregion
}

namespace S1CooldownTags
{

#pragma region Boss000
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Boss000_Attack00);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Boss000_Attack01);
#pragma endregion
}

namespace S1Labels
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Label_Preload);
}

namespace S1AssetTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_InputData);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_UIData);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_UIResource);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Ability_Player_Asna);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Ability_Boss000);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_AnimData);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Effect);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_PoolingData);
}

namespace S1StateTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CanNextAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Air);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Air_Used_WeakAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Air_Used_Skill01);
}

namespace S1EventTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_EarlyMove);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Landed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_StartDive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_CheckDistance);
}

namespace S1EffectTags
{
#pragma region Decal
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Decal_AttackRange);
#pragma endregion
}

namespace S1PoolingTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pool_Stage01);
}

namespace S1SetByCallerTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
}
