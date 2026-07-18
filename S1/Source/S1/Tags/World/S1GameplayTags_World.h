// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace S1PoolingTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pool_Stage01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pool_DropItem);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pool_Enemy001);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pool_Decal_AttackRange);
}

namespace S1SoundTags
{
#pragma region PlayerSound
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Player_Female_LevelUp);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Player_Male_LevelUp);
#pragma endregion

#pragma region HitSound
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Hit_RPR_Weak);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Hit_RPR_Strong);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Hit_RPR_Guard);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Hit_SWD_Weak);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Hit_SWD_Strong);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Hit_SWD_Guard);
#pragma endregion

#pragma region BossSound
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Boss_GuardBlock);
#pragma endregion

#pragma region GimmickSound
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Gimmick_Button_Press);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Gimmick_Pillar_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Gimmick_Plate_Step);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Gimmick_Door_Open);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Gimmick_Door_OpenComplete);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_Gimmick_Boss000Pillar_Break);
#pragma endregion

#pragma region UISound
	// leaf는 UI.Icon.Announce.* 의 leaf와 일치해야 함 — US1Announce::ShowAnnounce에서 조합해 조회.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_UI_Announce_BossCleared);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_UI_Announce_QuestCleared);
#pragma endregion

#pragma region BGM
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_BGM_Boss000_Battle);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_BGM_Boss000_Battle_Phase02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_BGM_Boss000_Battle_Fiver);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sound_BGM_Boss000_Field);
#pragma endregion
}

namespace S1EffectTags
{
#pragma region Decal
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Decal_AttackRange);
#pragma endregion

#pragma region PlayerEffect
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Slash01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Slash02);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Thrust01);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Wave01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Wave02);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Wave03);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Wave04);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Aura01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Aura02);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_StatUp);
#pragma endregion

#pragma region Impact
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Impact);
#pragma endregion

#pragma region DamageNumber
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_DamageNumber);
#pragma endregion

#pragma region Particle
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Particle_Fragment);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Particle_Dust);
#pragma endregion
}

namespace S1MonsterCollisionTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Collision_Attack_Head);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Collision_Attack_Right);
}

namespace S1HitLagTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitLag_PerfectDodge);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitLag_Ultimate);
}

namespace S1CharacterTag
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Charcter_Select_Asna);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Charcter_Select_Kirito);
}
