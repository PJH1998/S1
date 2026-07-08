// Fill out your copyright notice in the Description page of Project Settings.

#include "Tags/GAS/S1GameplayTags_GAS.h"

namespace S1AbilityTags
{
#pragma region Player
	#pragma region Move
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Move_Dash,  "Ability.Player.Move.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Move_Dodge, "Ability.Player.Move.Dodge");
	#pragma endregion

	#pragma region Attack
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack,					"Ability.Player.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Ground,			"Ability.Player.Attack.Ground");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Air,				"Ability.Player.Attack.Air");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_WeakAttack,		"Ability.Player.Attack.WeakAttack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_WeakAttackDive,	"Ability.Player.Attack.WeakAttackDive");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Skill01,			"Ability.Player.Attack.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Skill02,			"Ability.Player.Attack.Skill02");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Skill03,			"Ability.Player.Attack.Skill03");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Skill04,			"Ability.Player.Attack.Skill04");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Assault,			"Ability.Player.Attack.Assault");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Ultimiate,			"Ability.Player.Attack.Ultimiate");
	#pragma endregion
#pragma endregion

#pragma region Boss000
	UE_DEFINE_GAMEPLAY_TAG(Ability_Boss000_Encounter, "Ability.Boss000.Encounter");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Boss000_Attack00, "Ability.Boss000.Attack00");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Boss000_Attack01, "Ability.Boss000.Attack01");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Boss000_RotateAttack, "Ability.Boss000.RotateAttack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Boss000_JumpAttack, "Ability.Boss000.JumpAttack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Boss000_WallKick, "Ability.Boss000.WallKick");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Boss000_PhaseTransition, "Ability.Boss000.PhaseTransition");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Boss000_Guard, "Ability.Boss000.Guard");
#pragma endregion

#pragma region Enemy001
	UE_DEFINE_GAMEPLAY_TAG(Ability_Enemy001_Attack01, "Ability.Enemy001.Attack01");
#pragma endregion
}

namespace S1CooldownTags
{
#pragma region Player
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Player_Ground_Skill01, "Cooldown.Player.Ground.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Player_Air_Skill01,    "Cooldown.Player.Air.Skill01");
#pragma endregion

#pragma region Boss000
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Boss000_Attack00, "Cooldown.Boss000.Attack00");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Boss000_Attack01, "Cooldown.Boss000.Attack01");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Boss000_RotateAttack, "Cooldown.Boss000.RotateAttack");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Boss000_JumpAttack, "Cooldown.Boss000.JumpAttack");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Boss000_Wallkick, "Cooldown.Boss000.Wallkick");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Boss000_Guard, "Cooldown.Boss000.Guard");
#pragma endregion

#pragma region Enemy001
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Enemy001_Attack01, "Cooldown.Enemy001.Attack01");
#pragma endregion
}

namespace S1GameplayEffectTags
{
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_GainXP,     "GameplayEffect.GainXP");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_ApplyEquip, "GameplayEffect.ApplyEquip");
}

namespace S1StateTags
{
	UE_DEFINE_GAMEPLAY_TAG(State_Action,              "State.Action");
	UE_DEFINE_GAMEPLAY_TAG(State_CanNextAttack,       "State.CanNextAttack");
	UE_DEFINE_GAMEPLAY_TAG(State_Air,                 "State.Air");

	UE_DEFINE_GAMEPLAY_TAG(State_Used_Air,            "State.Used.Air");
	UE_DEFINE_GAMEPLAY_TAG(State_Used_Air_WeakAttack, "State.Used.Air.WeakAttack");
	UE_DEFINE_GAMEPLAY_TAG(State_Used_Air_Assault, "State.Used.Air.Assault");
	UE_DEFINE_GAMEPLAY_TAG(State_Used_Air_Skill01, "State.Used.Air.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(State_Used_Air_Skill02, "State.Used.Air.Skill02");
	UE_DEFINE_GAMEPLAY_TAG(State_Used_Air_Skill03, "State.Used.Air.Skill03");
	UE_DEFINE_GAMEPLAY_TAG(State_Used_Air_Skill04, "State.Used.Air.Skill04");

	UE_DEFINE_GAMEPLAY_TAG(State_Invincible,          "State.Invincible");
	UE_DEFINE_GAMEPLAY_TAG(State_Boss_Guard,          "State.Boss.Guard");
	UE_DEFINE_GAMEPLAY_TAG(State_Enemy_Attacking,     "State.Enemy.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(State_Enemy_Air,           "State.Enemy.Air");
	UE_DEFINE_GAMEPLAY_TAG(State_Hit_React,           "State.Hit.React");
	UE_DEFINE_GAMEPLAY_TAG(State_Hit_Launch,          "State.Hit.Launch");
	UE_DEFINE_GAMEPLAY_TAG(State_Hit_Knockdown,       "State.Hit.Knockdown");
	UE_DEFINE_GAMEPLAY_TAG(State_Hit_GetUp,           "State.Hit.GetUp");
}

namespace S1HitType
{
	UE_DEFINE_GAMEPLAY_TAG(HitType_Weak,	"HitType.Weak");
	UE_DEFINE_GAMEPLAY_TAG(HitType_Strong,	"HitType.Strong");
	UE_DEFINE_GAMEPLAY_TAG(HitType_ToAir,	"HitType.ToAir");
}

namespace S1EventTags
{
	UE_DEFINE_GAMEPLAY_TAG(Event_EarlyMove, "Event.EarlyMove");
	UE_DEFINE_GAMEPLAY_TAG(Event_Jump,      "Event.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Event_Landed,    "Event.Landed");

	UE_DEFINE_GAMEPLAY_TAG(Event_Enemy_Apex,   "Event.Enemy.Apex");
	UE_DEFINE_GAMEPLAY_TAG(Event_Enemy_Landed, "Event.Enemy.Landed");

	UE_DEFINE_GAMEPLAY_TAG(Event_Boss000_WeaponHide, "Event.Boss000.WeaponHide");
	UE_DEFINE_GAMEPLAY_TAG(Event_Boss000_WeaponDraw, "Event.Boss000.WeaponDraw");

	UE_DEFINE_GAMEPLAY_TAG(Event_LoopStart, "Event.LoopStart");
	UE_DEFINE_GAMEPLAY_TAG(Event_LoopCycle, "Event.LoopCycle");

	UE_DEFINE_GAMEPLAY_TAG(Event_CheckDistance, "Event.CheckDistance");

	UE_DEFINE_GAMEPLAY_TAG(Event_MovementStart, "Event.MovementStart");
	UE_DEFINE_GAMEPLAY_TAG(Event_MovementEnd,   "Event.MovementEnd");
}

namespace S1SetByCallerTags
{
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage,        "SetByCaller.Damage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_XP,            "SetByCaller.XP");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Status,        "SetByCaller.Status");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Status_Health, "SetByCaller.Status.Health");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Status_Damage, "SetByCaller.Status.Damage");
}
