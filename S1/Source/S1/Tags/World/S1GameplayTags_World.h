// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace S1PoolingTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pool_Stage01);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pool_DropItem);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pool_Enemy001);
}

namespace S1EffectTags
{
#pragma region Decal
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Decal_AttackRange);
#pragma endregion

#pragma region Slash
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Slash);
#pragma endregion

#pragma region Impact
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Impact);
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
}
