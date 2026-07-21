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

	#pragma region Action
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

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Action_HealItem);
	#pragma endregion

	#pragma region Hit
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Hit_Weak);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Hit_Strong);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Hit_ToAir);
	#pragma endregion

	#pragma region Death
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Death);
	#pragma endregion
#pragma endregion

#pragma region Boss000
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_Encounter);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_Attack00);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_Attack01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_RotateAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_JumpAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_WallKick);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_PhaseTransition);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_Guard);
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

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Player_Ground_Skill02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Player_Air_Skill02);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Player_Ground_Skill03);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Player_Air_Skill03);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Player_Ground_Skill04);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Player_Air_Skill04);
#pragma endregion

#pragma region Boss000
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Boss000_Attack00);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Boss000_Attack01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Boss000_RotateAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Boss000_JumpAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Boss000_Wallkick);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Boss000_Guard);
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

namespace S1HitType
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitType_Weak);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitType_Strong);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitType_ToAir);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitType_Guard);

	// 사망 시(HP<=0) 원래 피격 타입과 무관하게 이 태그 하나만 발송 — GA_Death는 애니메이션 제약상 단일 클래스
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitType_Death);
}

namespace S1StateTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CanNextAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CanUltimate);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Air);

	// 상위 태그 — State.Air 하위가 아니라 별도 트리라서(State.Used.Air.*), 착지 시 State.Air와 별개로 이 트리도 같이 정리해야 함
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Used_Air);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Used_Air_WeakAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Used_Air_Assault);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Used_Air_Skill01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Used_Air_Skill02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Used_Air_Skill03);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Used_Air_Skill04);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Invincible);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Boss_Guard);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Enemy_Attacking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Enemy_Air);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Hit_React);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Hit_Launch);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Hit_Knockdown);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Hit_GetUp);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);
	// Death 몽타주가 Loop 구간에 진입한 뒤에만 부여 — 이 태그가 있을 때만 리스폰 입력을 받는다.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead_CanRespawn);
}

namespace S1EventTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_EarlyMove);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Landed);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Enemy_Apex);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Enemy_Landed);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Boss000_WeaponHide);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Boss000_WeaponDraw);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_LoopStart);
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
