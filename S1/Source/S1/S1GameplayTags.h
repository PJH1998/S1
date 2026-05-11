// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace S1GameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Turn);
}

namespace S1UITags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Root);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Cursor);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Fade);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Lobby_HUD);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Lobby_Logo);
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
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_UIData);
#pragma endregion
}