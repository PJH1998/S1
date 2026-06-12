// Fill out your copyright notice in the Description page of Project Settings.


#include "S1GameplayTags.h"

namespace S1GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move, "Input.Action.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Sprint, "Input.Action.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Turn, "Input.Action.Turn");

	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Attack, "Input.Action.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Jump, "Input.Action.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Dash, "Input.Action.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Dodge, "Input.Action.Dodge");

	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Skill01, "Input.Action.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Skill02, "Input.Action.Skill02");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Skill03, "Input.Action.Skill03");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Assault, "Input.Action.Assault");

	UE_DEFINE_GAMEPLAY_TAG(Input_Action_LockOn,           "Input.Action.LockOn");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_ChangeLockOnSide,  "Input.Action.ChangeLockOnSide");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_LockOnLeft,         "Input.Action.LockOnLeft");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_LockOnRight,        "Input.Action.LockOnRight");

#pragma region UI
	UE_DEFINE_GAMEPLAY_TAG(Input_UI_Inventory, "Input.UI.Inventory");

#pragma endregion
}

namespace S1UITags
{
	UE_DEFINE_GAMEPLAY_TAG(UI_Root, "UI.Root");
	UE_DEFINE_GAMEPLAY_TAG(UI_Cursor, "UI.Cursor");
	UE_DEFINE_GAMEPLAY_TAG(UI_Fade, "UI.Fade");

	UE_DEFINE_GAMEPLAY_TAG(UI_Lobby_HUD, "UI.Lobby.HUD");
	UE_DEFINE_GAMEPLAY_TAG(UI_Lobby_Logo, "UI.Lobby.Logo");
	UE_DEFINE_GAMEPLAY_TAG(UI_Gameplay_HUD, "UI.Gameplay.HUD");

	UE_DEFINE_GAMEPLAY_TAG(UI_Menu_Inventory, "UI.Menu.Inventory");
	UE_DEFINE_GAMEPLAY_TAG(UI_Menu_ItemInfo, "UI.Menu.ItemInfo");
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
}

namespace S1AnimTags
{
#pragma region Player_Common
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_Common_Ground_Dash, "Anim.Montage.Player.Common.Ground.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_Common_Ground_Dodge, "Anim.Montage.Player.Common.Ground.Dodge");

	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_Common_Air_Dash, "Anim.Montage.Player.Common.Air.Dash");
#pragma endregion

#pragma region Player_RPR
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Ground_Weak, "Anim.Montage.Player.RPR.Ground.Weak");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Ground_Assault, "Anim.Montage.Player.RPR.Ground.Assault");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Ground_Skill01, "Anim.Montage.Player.RPR.Ground.Skill01");

	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Air_Weak, "Anim.Montage.Player.RPR.Air.Weak");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Air_WeakDive, "Anim.Montage.Player.RPR.Air.WeakDive");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Air_Skill01, "Anim.Montage.Player.RPR.Air.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Air_Assault, "Anim.Montage.Player.RPR.Air.Assault");
#pragma endregion

#pragma region Player_SWD
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_SWD_Ground_Weak, "Anim.Montage.Player.SWD.Ground.Weak");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_SWD_Ground_Skill01, "Anim.Montage.Player.SWD.Ground.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_SWD_Ground_Assault, "Anim.Montage.Player.SWD.Ground.Assault");

	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_SWD_Air_Weak, "Anim.Montage.Player.SWD.Air.Weak");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_SWD_Air_Skill01, "Anim.Montage.Player.SWD.Air.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_SWD_Air_Assault, "Anim.Montage.Player.SWD.Air.Assault");
#pragma endregion

#pragma region Monster_Boss000
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Boss000_Atack00, "Anim.Montage.Boss000.Attack00");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Boss000_Atack01, "Anim.Montage.Boss000.Attack01");
#pragma endregion
}

namespace S1AbilityTags
{
#pragma region Player_Common
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Common_Dash, "Ability.Player.Common.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Common_Dodge, "Ability.Player.Common.Dodge");
#pragma endregion

#pragma region Player_Attack
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack, "Ability.Player.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Ground, "Ability.Player.Attack.Ground");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Air, "Ability.Player.Attack.Air");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_WeakAttack, "Ability.Player.Attack.WeakAttack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_WeakAttackDive, "Ability.Player.Attack.WeakAttackDive");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Skill01, "Ability.Player.Attack.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Skill02, "Ability.Player.Attack.Skill02");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Skill03, "Ability.Player.Attack.Skill03");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Assault, "Ability.Player.Attack.Assault");
#pragma endregion

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
#pragma region Player
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Player_Ground_Skill01, "Cooldown.Player.Ground.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Player_Air_Attack01, "Cooldown.Player.Air.Skill01");
#pragma endregion

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
#pragma region Input
	UE_DEFINE_GAMEPLAY_TAG(Asset_InputData, "Asset.InputData");
#pragma endregion

#pragma region UI
	UE_DEFINE_GAMEPLAY_TAG(Asset_UIData, "Asset.UIData");
	UE_DEFINE_GAMEPLAY_TAG(Asset_UIResource, "Asset.UIResource");
#pragma endregion

#pragma region GameAbilitySystem

	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability_Player, "Asset.Ability.Player");

	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability_Boss000,"Asset.Ability.Boss000");

	UE_DEFINE_GAMEPLAY_TAG(Asset_GameplayEffect, "Asset.GameplayEffect");
#pragma endregion

#pragma region Animation
	UE_DEFINE_GAMEPLAY_TAG(Asset_AnimData, "Asset.AnimData");
#pragma endregion

#pragma region Effect
	UE_DEFINE_GAMEPLAY_TAG(Asset_Effect, "Asset.Effect");
#pragma endregion

#pragma region Datas
	UE_DEFINE_GAMEPLAY_TAG(Asset_PoolingData, "Asset.PoolingData");

	UE_DEFINE_GAMEPLAY_TAG(Asset_DataTable, "Asset.DataTable");

	UE_DEFINE_GAMEPLAY_TAG(Asset_DropItemResource, "Asset.DropItemResource");
	UE_DEFINE_GAMEPLAY_TAG(Asset_DropTable_Boss000, "Asset.DropTable.Boss000");

	UE_DEFINE_GAMEPLAY_TAG(Asset_WeaponData, "Asset.WeaponData");
#pragma endregion 
}

namespace S1GameplayEffectTags
{
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_GainXP, "GameplayEffect.GainXP");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_ApplyEquip, "GameplayEffect.ApplyEquip");
}

namespace S1StateTags
{
	UE_DEFINE_GAMEPLAY_TAG(State_Action, "State.Action");
	UE_DEFINE_GAMEPLAY_TAG(State_CanNextAttack, "State.CanNextAttack");
	UE_DEFINE_GAMEPLAY_TAG(State_Air, "State.Air");
	UE_DEFINE_GAMEPLAY_TAG(State_Air_Used_WeakAttack, "State.Air.Used.WeakAttack");
	UE_DEFINE_GAMEPLAY_TAG(State_Air_Used_Skill01, "State.Air.Used.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(State_Invincible, "State.Invincible");
}

namespace S1EventTags
{
	UE_DEFINE_GAMEPLAY_TAG(Event_EarlyMove, "Event.EarlyMove");

	UE_DEFINE_GAMEPLAY_TAG(Event_Jump, "Event.Jump");

	UE_DEFINE_GAMEPLAY_TAG(Event_Landed, "Event.Landed");

	UE_DEFINE_GAMEPLAY_TAG(Event_StartDive, "Event.StartDive");
	UE_DEFINE_GAMEPLAY_TAG(Event_LoopCycle, "Event.LoopCycle");

	UE_DEFINE_GAMEPLAY_TAG(Event_CheckDistance, "Event.CheckDistance");

	UE_DEFINE_GAMEPLAY_TAG(Event_MovementStart, "Event.MovementStart");
	UE_DEFINE_GAMEPLAY_TAG(Event_MovementEnd, "Event.MovementEnd");

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
	UE_DEFINE_GAMEPLAY_TAG(Pool_DropItem, "Pool.DropItem");
}

namespace S1DropTags
{
	UE_DEFINE_GAMEPLAY_TAG(Drop_Gold, "Drop.Gold");
	UE_DEFINE_GAMEPLAY_TAG(Drop_Exp, "Drop.Exp");
}

namespace S1ItemTags
{
	UE_DEFINE_GAMEPLAY_TAG(Item_Weapon_RPR01, "Item.Weapon.RPR01");
	UE_DEFINE_GAMEPLAY_TAG(Item_Weapon_RPR02, "Item.Weapon.RPR02");
	UE_DEFINE_GAMEPLAY_TAG(Item_Weapon_RPR03, "Item.Weapon.RPR03");
	UE_DEFINE_GAMEPLAY_TAG(Item_Weapon_RPR04, "Item.Weapon.RPR04");

	UE_DEFINE_GAMEPLAY_TAG(Item_Weapon_SWD01, "Item.Weapon.SWD01");
	UE_DEFINE_GAMEPLAY_TAG(Item_Weapon_SWD02, "Item.Weapon.SWD02");
	UE_DEFINE_GAMEPLAY_TAG(Item_Weapon_SWD03, "Item.Weapon.SWD03");
	UE_DEFINE_GAMEPLAY_TAG(Item_Weapon_SWD04, "Item.Weapon.SWD04");
}

namespace S1ItemRarityTags
{
	UE_DEFINE_GAMEPLAY_TAG(Item_Rarity_Common, "Item.Rarity.Common");
	UE_DEFINE_GAMEPLAY_TAG(Item_Rarity_Rare, "Item.Rarity.Rare");
	UE_DEFINE_GAMEPLAY_TAG(Item_Rarity_Unique, "Item.Rarity.Unique");
	UE_DEFINE_GAMEPLAY_TAG(Item_Rarity_Legendary, "Item.Rarity.Legendary");
}

namespace S1ItemTypeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Item_Type_Equip, "Item.Type.Equip");
	UE_DEFINE_GAMEPLAY_TAG(Item_Type_Inv, "Item.Type.Inv");
}

namespace S1EquipSlotTags
{
	UE_DEFINE_GAMEPLAY_TAG(Equip_Type_Weapon, "Equip.Type.Weapon");
	UE_DEFINE_GAMEPLAY_TAG(Equip_Type_Costume, "Equip.Type.Costume");
	UE_DEFINE_GAMEPLAY_TAG(Equip_Type_Accessary, "Equip.Type.Accessary");
}

namespace S1SetByCallerTags
{
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage, "SetByCaller.Damage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_XP, "SetByCaller.XP");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Status, "SetByCaller.Status");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Status_Health, "SetByCaller.Status.Health");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Status_Damage, "SetByCaller.Status.Damage");
}

namespace S1DataTableTags
{
	UE_DEFINE_GAMEPLAY_TAG(DataTable_PlayerData, "DataTable.PlayerData");
	UE_DEFINE_GAMEPLAY_TAG(DataTable_MonsterData, "DataTable.MonsterData");
	UE_DEFINE_GAMEPLAY_TAG(DataTable_ItemData, "DataTable.ItemData");
	UE_DEFINE_GAMEPLAY_TAG(DataTable_HitLagData, "DataTable.HitLagData");
}

namespace S1HitLagTags
{
	UE_DEFINE_GAMEPLAY_TAG(HitLag_PerfectDodge, "HitLag.PerfectDodge");
}
