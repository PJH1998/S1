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
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Dash);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Dodge);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Skill01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Skill02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Skill03);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Assault);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_LockOn);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_ChangeLockOnSide);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_LockOnLeft);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_LockOnRight);

#pragma region UI
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_UI_Inventory);

#pragma endregion
}

namespace S1UITags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Root);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Cursor);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Fade);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Lobby_HUD);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Lobby_Logo);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Gameplay_HUD);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Menu_Inventory);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Menu_ItemInfo);
}

namespace S1UIResourceTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Tex_Heal);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Tex_Asna_Skill01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Tex_Asna_Skill02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Tex_Asna_Skill03);

#pragma region Inventory
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Icon_BG_Common);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Icon_BG_Rare);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Icon_BG_Unique);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Icon_BG_Legendary);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Icon_Frame_Common);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Icon_Frame_Rare);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Icon_Frame_Unique);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Icon_Frame_Legendary);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Icon_Effect_Equiped);
#pragma endregion

}

namespace S1AnimTags
{
#pragma region Player
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_RPR_Weak);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_RPR_AirWeak);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_RPR_AirWeakDive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_RPR_Skill01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_RPR_Dash);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_RPR_AirDash);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_RPR_Dodge);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_RPR_Assault);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_RPR_AirAssault);

#pragma endregion

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_Boss000_Atack00);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anim_Montage_Boss000_Atack01);
}

namespace S1AbilityTags
{
#pragma region Player
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Ground);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Air);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_WeakAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_WeakAttackDive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Skill01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Skill02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Skill03);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Assault);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Dash);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Dodge);
#pragma endregion

#pragma region Boss000
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_Attack00);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss000_Attack01);
#pragma endregion
}

namespace S1MonsterCollisionTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Collision_Attack_Right);
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
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_DataTable);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_DropItemResource);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_DropTable_Boss000);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_GameplayEffect);
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
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_CheckDistance);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_MovementStart);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_MovementEnd);
}

namespace S1EffectTags
{
#pragma region Decal
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Decal_AttackRange);
#pragma endregion

#pragma region Slash
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Slash);
#pragma endregion

#pragma region Impact
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Impact);
#pragma endregion
}

namespace S1PoolingTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pool_Stage01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pool_DropItem);
}

namespace S1DropTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Drop_Gold);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Drop_Exp);
}

namespace S1ItemTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Weapon_RPR01);
}

namespace S1ItemRarityTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity_Common);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity_Rare);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity_Unique);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity_Legendary);
}

namespace S1ItemTypeTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Type_Equip);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Type_Inv);
}

namespace S1EquipSlotTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equip_Type_Weapon);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equip_Type_Costume);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equip_Type_Accessary);
}

namespace S1SetByCallerTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_XP);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Status);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Status_Health);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Status_Damage);
}

namespace S1DataTableTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataTable_PlayerData);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataTable_MonsterData);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataTable_ItemData);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataTable_HitLagData);
}

namespace S1HitLagTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitLag_PerfectDodge);
}
