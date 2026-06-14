// Fill out your copyright notice in the Description page of Project Settings.

#include "Tags/Item/S1GameplayTags_Item.h"

namespace S1DropTags
{
	UE_DEFINE_GAMEPLAY_TAG(Drop_Gold, "Drop.Gold");
	UE_DEFINE_GAMEPLAY_TAG(Drop_Exp,  "Drop.Exp");
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
	UE_DEFINE_GAMEPLAY_TAG(Item_Rarity_Common,    "Item.Rarity.Common");
	UE_DEFINE_GAMEPLAY_TAG(Item_Rarity_Rare,      "Item.Rarity.Rare");
	UE_DEFINE_GAMEPLAY_TAG(Item_Rarity_Unique,    "Item.Rarity.Unique");
	UE_DEFINE_GAMEPLAY_TAG(Item_Rarity_Legendary, "Item.Rarity.Legendary");
}

namespace S1ItemTypeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Item_Type_Equip, "Item.Type.Equip");
	UE_DEFINE_GAMEPLAY_TAG(Item_Type_Inv,   "Item.Type.Inv");
}

namespace S1EquipSlotTags
{
	UE_DEFINE_GAMEPLAY_TAG(Equip_Type_Weapon,    "Equip.Type.Weapon");
	UE_DEFINE_GAMEPLAY_TAG(Equip_Type_Costume,   "Equip.Type.Costume");
	UE_DEFINE_GAMEPLAY_TAG(Equip_Type_Accessary, "Equip.Type.Accessary");
}
