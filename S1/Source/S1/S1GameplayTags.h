// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace S1GameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Turn);
}

namespace S1DataTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Input);
}

namespace S1AssetTags
{
#pragma region LABEL
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Label_Preload);
#pragma endregion


#pragma region ASSET
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_InputData);
#pragma endregion
}