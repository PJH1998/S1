// Fill out your copyright notice in the Description page of Project Settings.

#include "Tags/Input/S1GameplayTags_Input.h"

namespace S1GameplayTags
{
#pragma region Player
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
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Skill04, "Input.Action.Skill04");

	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Assault, "Input.Action.Assault");

	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ultimate, "Input.Action.Ultimate");

	UE_DEFINE_GAMEPLAY_TAG(Input_Action_LockOn,				"Input.Action.LockOn");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_ChangeLockOnSide,	"Input.Action.ChangeLockOnSide");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_LockOnLeft,			"Input.Action.LockOnLeft");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_LockOnRight,		"Input.Action.LockOnRight");
#pragma endregion

#pragma region UI
	UE_DEFINE_GAMEPLAY_TAG(Input_UI_Inventory, "Input.UI.Inventory");
	UE_DEFINE_GAMEPLAY_TAG(Input_UI_FreeCursor, "Input.UI.FreeCursor");
#pragma endregion
}
