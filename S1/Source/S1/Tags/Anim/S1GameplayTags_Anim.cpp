// Fill out your copyright notice in the Description page of Project Settings.

#include "Tags/Anim/S1GameplayTags_Anim.h"

namespace S1AnimTags
{
#pragma region Assault_Branch
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Female_RPR_Assault_End_Air,    "Anim.Montage.Female.RPR.AssaultEnd.Air");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Female_RPR_Assault_End_Ground, "Anim.Montage.Female.RPR.AssaultEnd.Ground");

	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Female_SWD_AssaultEnd_Air, "Anim.Montage.Female.SWD.AssaultEnd.Air");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Female_SWD_AssaultEnd_Ground, "Anim.Montage.Female.SWD.AssaultEnd.Ground");

#pragma endregion

#pragma region SWD_Skill_Branch
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Female_SWD_Skill02_End_Weak, "Anim.Montage.Female.SWD.Skill02End.Weak");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Female_SWD_Skill02_End_Strong, "Anim.Montage.Female.SWD.Skill02End.Strong");
#pragma endregion

#pragma region Monster_Boss000
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Boss000_Atack00, "Anim.Montage.Boss000.Attack00");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Boss000_Atack01, "Anim.Montage.Boss000.Attack01");
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Boss000_RotateAttack, "Anim.Montage.Boss000.RotateAttack");
#pragma endregion

#pragma region Enemy001
	UE_DEFINE_GAMEPLAY_TAG(Anim_Montage_Enemy001_Attack01, "Anim.Montage.Enemy001.Attack01");
#pragma endregion
}
