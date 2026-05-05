// Fill out your copyright notice in the Description page of Project Settings.


#include "S1GameplayTags.h"

namespace S1GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move, "Input.Action.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Turn, "Input.Action.Turn");
}

namespace S1DataTags
{
	UE_DEFINE_GAMEPLAY_TAG(Data_Input, "Data.Input");
}

namespace S1AssetTags
{
#pragma region LABEL
	UE_DEFINE_GAMEPLAY_TAG(Label_Preload, "Label.Preload");
#pragma endregion

#pragma region ASSET
	UE_DEFINE_GAMEPLAY_TAG(Asset_InputData, "Asset.InputData");
#pragma endregion
}