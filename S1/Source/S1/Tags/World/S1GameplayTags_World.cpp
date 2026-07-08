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
	UE_DEFINE_GAMEPLAY_TAG(Effect_Player_Slash01, "Effect.Player.Slash01");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Player_Slash02, "Effect.Player.Slash02");

	UE_DEFINE_GAMEPLAY_TAG(Effect_Player_Thrust01, "Effect.Player.Thrust01");

	UE_DEFINE_GAMEPLAY_TAG(Effect_Player_Wave01, "Effect.Player.Wave01");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Player_Wave02, "Effect.Player.Wave02");

	UE_DEFINE_GAMEPLAY_TAG(Effect_Player_Aura01, "Effect.Player.Aura01");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Player_Aura02, "Effect.Player.Aura02");
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
