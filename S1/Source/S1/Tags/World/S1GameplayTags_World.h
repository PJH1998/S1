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

namespace S1EffectTags
{
#pragma region Decal
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Decal_AttackRange);
#pragma endregion

#pragma region RPR
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Slash01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Slash02);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Thrust01);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Wave01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Wave02);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Aura01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Player_Aura02);
#pragma endregion

#pragma region SWD
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
