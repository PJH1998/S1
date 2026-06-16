// Fill out your copyright notice in the Description page of Project Settings.

#include "Tags/World/S1GameplayTags_World.h"

namespace S1PoolingTags
{
	UE_DEFINE_GAMEPLAY_TAG(Pool_Stage01,  "Pool.Stage01");
	UE_DEFINE_GAMEPLAY_TAG(Pool_DropItem, "Pool.DropItem");
	UE_DEFINE_GAMEPLAY_TAG(Pool_Enemy001, "Pool.Enemy001");
}

namespace S1EffectTags
{
#pragma region Decal
	UE_DEFINE_GAMEPLAY_TAG(Effect_Decal_AttackRange, "Effect.Decal.AttackRange");
#pragma endregion

#pragma region Slash
	UE_DEFINE_GAMEPLAY_TAG(Effect_Slash, "Effect.Slash");
#pragma endregion

#pragma region Impact
	UE_DEFINE_GAMEPLAY_TAG(Effect_Impact, "Effect.Impact");
#pragma endregion
}

namespace S1MonsterCollisionTags
{
	UE_DEFINE_GAMEPLAY_TAG(Collision_Attack_Right, "Collision.Attack.Right");
}

namespace S1HitLagTags
{
	UE_DEFINE_GAMEPLAY_TAG(HitLag_PerfectDodge, "HitLag.PerfectDodge");
	UE_DEFINE_GAMEPLAY_TAG(HitLag_Ultimate, "HitLag.Ultimate");
}
