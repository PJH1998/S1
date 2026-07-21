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

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_HealItem);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_LockOn);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_ChangeLockOnSide);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_LockOnLeft);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_LockOnRight);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Interact);

	// State.Dead.CanRespawn 있을 때만 반응 — 키 또는 UI 버튼 둘 다 AS1PlayerController::TryRespawn()으로 수렴
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Respawn);
#pragma endregion

#pragma region UI
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_UI_Inventory);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_UI_FreeCursor);
#pragma endregion
}
