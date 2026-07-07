// Fill out your copyright notice in the Description page of Project Settings.

#include "Tags/World/S1GameplayTags_World.h"

namespace S1PoolingTags
{
	UE_DEFINE_GAMEPLAY_TAG(Pool_Stage01,  "Pool.Stage01");
	UE_DEFINE_GAMEPLAY_TAG(Pool_DropItem, "Pool.DropItem");
	UE_DEFINE_GAMEPLAY_TAG(Pool_Enemy001, "Pool.Enemy001");
	UE_DEFINE_GAMEPLAY_TAG(Pool_Decal_AttackRange, "Pool.Decal.AttackRange");
}

namespace S1EffectTags
{
#pragma region Decal
	UE_DEFINE_GAMEPLAY_TAG(Effect_Decal_AttackRange, "Effect.Decal.AttackRange");
#pragma endregion

#pragma region RPR
	UE_DEFINE_GAMEPLAY_TAG(Effect_RPR_Slash, "Effect.RPR.Slash");
	UE_DEFINE_GAMEPLAY_TAG(Effect_RPR_Thrust, "Effect.RPR.Thrust");
#pragma endregion

#pragma region SWD
#pragma endregion

#pragma region Impact
	UE_DEFINE_GAMEPLAY_TAG(Effect_Impact, "Effect.Impact");
#pragma endregion

#pragma region DamageNumber
	UE_DEFINE_GAMEPLAY_TAG(Effect_DamageNumber, "Effect.DamageNumber");
#pragma endregion

#pragma region Particle
	UE_DEFINE_GAMEPLAY_TAG(Effect_Particle_Fragment, "Effect.Particle.Fragment");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Particle_Dust, "Effect.Particle.Dust");
#pragma endregion
}

namespace S1MonsterCollisionTags
{
	UE_DEFINE_GAMEPLAY_TAG(Collision_Attack_Head, "Collision.Attack.Head");
	UE_DEFINE_GAMEPLAY_TAG(Collision_Attack_Right, "Collision.Attack.Right");
}

namespace S1HitLagTags
{
	UE_DEFINE_GAMEPLAY_TAG(HitLag_PerfectDodge, "HitLag.PerfectDodge");
	UE_DEFINE_GAMEPLAY_TAG(HitLag_Ultimate, "HitLag.Ultimate");
}

namespace S1CharacterTag
{
	UE_DEFINE_GAMEPLAY_TAG(Charcter_Select_Asna, "Character.Select.Asna");
	UE_DEFINE_GAMEPLAY_TAG(Charcter_Select_Kirito, "Character.Select.Kirito");
}
