// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace S1GameplayTags
{
#pragma region Player
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
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Skill04);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Assault);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_LockOn);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_ChangeLockOnSide);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_LockOnLeft);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_LockOnRight);
#pragma endregion

#pragma region UI
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_UI_Inventory);
#pragma endregion
}
