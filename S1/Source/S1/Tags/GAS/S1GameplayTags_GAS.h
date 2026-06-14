// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace S1AbilityTags
{
#pragma region Player
	#pragma region Move
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Move_Dash);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Move_Dodge);
	#pragma endregion

	#pragma region Attack
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Ground);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Air);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_WeakAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_WeakAttackDive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Skill01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Skill02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Skill03);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Skill04);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Assault);
	#pragma endregion
#pragma endregion

#pragma region Boss000
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_Attack00);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_Attack01);
#pragma endregion

#pragma region Enemy001
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Enemy001_Attack01);
#pragma endregion
}

namespace S1CooldownTags
{
#pragma region Player
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Player_Ground_Skill01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Player_Air_Skill01);
#pragma endregion

#pragma region Boss000
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Boss000_Attack00);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Boss000_Attack01);
#pragma endregion

#pragma region Enemy001
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Enemy001_Attack01);
#pragma endregion
}

namespace S1GameplayEffectTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_GainXP);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_ApplyEquip);
}

namespace S1StateTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CanNextAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Air);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Air_Used_WeakAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Air_Used_Skill01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Invincible);
}

namespace S1EventTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_EarlyMove);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Landed);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_StartDive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_LoopCycle);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_CheckDistance);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_MovementStart);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_MovementEnd);
}

namespace S1SetByCallerTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_XP);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Status);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Status_Health);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Status_Damage);
}
