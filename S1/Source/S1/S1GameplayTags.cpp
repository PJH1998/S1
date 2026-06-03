// Fill out your copyright notice in the Description page of Project Settings.


#include "S1GameplayTags.h"

namespace S1GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move, "Input.Action.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Sprint, "Input.Action.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Turn, "Input.Action.Turn");
	
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Attack, "Input.Action.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Jump, "Input.Action.Jump");

	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Skill01, "Input.Action.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Skill02, "Input.Action.Skill02");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Skill03, "Input.Action.Skill03");

	UE_DEFINE_GAMEPLAY_TAG(Input_Action_LockOn,           "Input.Action.LockOn");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_ChangeLockOnSide,  "Input.Action.ChangeLockOnSide");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_LockOnLeft,         "Input.Action.LockOnLeft");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_LockOnRight,        "Input.Action.LockOnRight");
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
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_AirWeak, "Anim.Montage.PRP_AirWeak");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_AirWeakDive, "Anim.Montage.PRP_AirWeakDive");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_Skill01, "Anim.Montage.RPR_Skill01");

	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Boss000_Atack00, "Anim.Montage.Boss000.Attack00");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Boss000_Atack01, "Anim.Montage.Boss000.Attack01");
}

namespace S1AbilityTags
{
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Ground, "Ability.Player.Attack.Ground");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Air, "Ability.Player.Attack.Air");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_WeakAttack, "Ability.Player.Attack.WeakAttack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_WeakAttackDive, "Ability.Player.Attack.WeakAttackDive");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Skill01, "Ability.Player.Attack.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Skill02, "Ability.Player.Attack.Skill02");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Skill03, "Ability.Player.Attack.Skill03");

#pragma region Boss000
	UE_DEFINE_GAMEPLAY_TAG(Ability_Boss000_Attack00, "Ability.Boss000.Attack00");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Boss000_Attack01, "Ability.Boss000.Attack01");
#pragma endregion
}

namespace S1MonsterCollisionTags
{
	UE_DEFINE_GAMEPLAY_TAG(Collision_Attack_Right, "Collision.Attack.Right");
}

namespace S1CooldownTags
{

#pragma region Boss000
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Boss000_Attack00, "Cooldown.Boss000.Attack00");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Boss000_Attack01, "Cooldown.Boss000.Attack01");
#pragma endregion
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
	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability_Player_Asna,"Asset.Ability.Player.Asna");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability_Boss000,"Asset.Ability.Boss000");
	UE_DEFINE_GAMEPLAY_TAG(Asset_AnimData, "Asset.AnimData");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Effect, "Asset.Effect");
	UE_DEFINE_GAMEPLAY_TAG(Asset_PoolingData, "Asset.PoolingData");
}

namespace S1StateTags
{
	UE_DEFINE_GAMEPLAY_TAG(State_Action, "State.Action");
	UE_DEFINE_GAMEPLAY_TAG(State_CanNextAttack, "State.CanNextAttack");
	UE_DEFINE_GAMEPLAY_TAG(State_Air, "State.Air");
	UE_DEFINE_GAMEPLAY_TAG(State_Air_Used_WeakAttack, "State.Air.Used.WeakAttack");
	UE_DEFINE_GAMEPLAY_TAG(State_Air_Used_Skill01, "State.Air.Used.Skill01");
}

namespace S1EventTags
{
	UE_DEFINE_GAMEPLAY_TAG(Event_EarlyMove, "Event.EarlyMove");
	UE_DEFINE_GAMEPLAY_TAG(Event_Jump, "Event.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Event_Landed, "Event.Landed");
	UE_DEFINE_GAMEPLAY_TAG(Event_StartDive, "Event.StartDive");
	UE_DEFINE_GAMEPLAY_TAG(Event_CheckDistance, "Event.CheckDistance");
}

namespace S1EffectTags
{
#pragma region Decal
	UE_DEFINE_GAMEPLAY_TAG(Effect_Decal_AttackRange, "Effect.Decal.AttackRange");
#pragma endregion

#pragma region Slash
	UE_DEFINE_GAMEPLAY_TAG(Effect_Slash, "Effect.Slash");
#pragma endregion

#pragma region Impact
	UE_DEFINE_GAMEPLAY_TAG(Effect_Impact, "Effect.Impact");
#pragma endregion
}

namespace S1PoolingTags
{
	UE_DEFINE_GAMEPLAY_TAG(Pool_Stage01, "Pool.Stage01");
}

namespace S1SetByCallerTags
{
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage, "SetByCaller.Damage");
}
