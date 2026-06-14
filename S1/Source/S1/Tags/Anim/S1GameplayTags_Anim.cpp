// Fill out your copyright notice in the Description page of Project Settings.

#include "Tags/Anim/S1GameplayTags_Anim.h"

namespace S1AnimTags
{
#pragma region RPR_Assault_Branch
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_Assault_End_Air,    "Anim.Montage.RPR.Assault.End.Air");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_RPR_Assault_End_Ground, "Anim.Montage.RPR.Assault.End.Ground");
#pragma endregion

#pragma region Player_Common
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_Common_Ground_Dash,  "Anim.Montage.Player.Common.Ground.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_Common_Ground_Dodge, "Anim.Montage.Player.Common.Ground.Dodge");

	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_Common_Air_Dash, "Anim.Montage.Player.Common.Air.Dash");
#pragma endregion

#pragma region Player_RPR
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Ground_Weak,    "Anim.Montage.Player.RPR.Ground.Weak");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Ground_Skill01, "Anim.Montage.Player.RPR.Ground.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Ground_Assault, "Anim.Montage.Player.RPR.Ground.Assault");

	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Air_Weak,     "Anim.Montage.Player.RPR.Air.Weak");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Air_WeakDive, "Anim.Montage.Player.RPR.Air.WeakDive");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Air_Skill01,  "Anim.Montage.Player.RPR.Air.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_RPR_Air_Assault,  "Anim.Montage.Player.RPR.Air.Assault");
#pragma endregion

#pragma region Player_SWD
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_SWD_Ground_Weak,    "Anim.Montage.Player.SWD.Ground.Weak");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_SWD_Ground_Skill01, "Anim.Montage.Player.SWD.Ground.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_SWD_Ground_Assault, "Anim.Montage.Player.SWD.Ground.Assault");

	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_SWD_Air_Weak,    "Anim.Montage.Player.SWD.Air.Weak");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_SWD_Air_Skill01, "Anim.Montage.Player.SWD.Air.Skill01");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Player_SWD_Air_Assault, "Anim.Montage.Player.SWD.Air.Assault");
#pragma endregion

#pragma region Monster_Boss000
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Boss000_Atack00, "Anim.Montage.Boss000.Attack00");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Boss000_Atack01, "Anim.Montage.Boss000.Attack01");
#pragma endregion

#pragma region Enemy001
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Enemy001_Attack01, "Anim.Montage.Enemy001.Attack01");
#pragma endregion
}
