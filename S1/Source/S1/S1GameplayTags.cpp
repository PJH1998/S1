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
#pragma region Player
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_Weak, "Anim.Montage.PRP_Weak");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_AirWeak, "Anim.Montage.PRP_AirWeak");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_AirWeakDive, "Anim.Montage.PRP_AirWeakDive");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_Skill01, "Anim.Montage.RPR_Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_Dash, "Anim.Montage.RPR_Dash");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_AirDash, "Anim.Montage.RPR_AirDash");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_Dodge, "Anim.Montage.RPR_Dodge");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_Assault, "Anim.Montage.RPR_Assault");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_AirAssault, "Anim.Montage.RPR_AirAssault");
#pragma endregion

	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Boss000_Atack00, "Anim.Montage.Boss000.Attack00");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Boss000_Atack01, "Anim.Montage.Boss000.Attack01");
}

namespace S1AbilityTags
{
#pragma region Player
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack, "Ability.Player.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Ground, "Ability.Player.Attack.Ground");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Air, "Ability.Player.Attack.Air");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_WeakAttack, "Ability.Player.Attack.WeakAttack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_WeakAttackDive, "Ability.Player.Attack.WeakAttackDive");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Skill01, "Ability.Player.Attack.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Skill02, "Ability.Player.Attack.Skill02");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Skill03, "Ability.Player.Attack.Skill03");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Assault, "Ability.Player.Attack.Assault");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Dash, "Ability.Player.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Dodge, "Ability.Player.Dodge");
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
	UE_DEFINE_GAMEPLAY_TAG(Asset_DataTable, "Asset.DataTable");
	UE_DEFINE_GAMEPLAY_TAG(Asset_DropItemResource, "Asset.DropItemResource");

	UE_DEFINE_GAMEPLAY_TAG(Asset_DropTable_Boss000, "Asset.DropTable.Boss000");
	UE_DEFINE_GAMEPLAY_TAG(Asset_GameplayEffect, "Asset.GameplayEffect");
}

namespace S1GameplayEffectTags
{
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_GainXP, "GameplayEffect.GainXP");
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
	UE_DEFINE_GAMEPLAY_TAG(Item_Weapon_Asna01, "Item.Weapon.Asna01");
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
