// Fill out your copyright notice in the Description page of Project Settings.


#include "S1GameplayTags.h"

namespace S1GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move, "Input.Action.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Turn, "Input.Action.Turn");
}

namespace S1AbilityTags
{
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack, "Ability.Attack");
}

namespace S1Labels
{
	UE_DEFINE_GAMEPLAY_TAG(Label_Preload, "Label.Preload");
}

namespace S1AssetTags
{
	UE_DEFINE_GAMEPLAY_TAG(Asset_InputData, "Asset.InputData");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Ability,"Asset.Ability");
}