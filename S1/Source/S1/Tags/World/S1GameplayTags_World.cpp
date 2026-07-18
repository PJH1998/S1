// Fill out your copyright notice in the Description page of Project Settings.

#include "Tags/World/S1GameplayTags_World.h"

namespace S1PoolingTags
{
	UE_DEFINE_GAMEPLAY_TAG(Pool_Stage01,  "Pool.Stage01");
	UE_DEFINE_GAMEPLAY_TAG(Pool_DropItem, "Pool.DropItem");
	UE_DEFINE_GAMEPLAY_TAG(Pool_Enemy001, "Pool.Enemy001");
	UE_DEFINE_GAMEPLAY_TAG(Pool_Decal_AttackRange, "Pool.Decal.AttackRange");
}

namespace S1SoundTags
{
#pragma region HitSound
	UE_DEFINE_GAMEPLAY_TAG(Sound_Hit_RPR_Weak, "Sound.Hit.RPR.Weak");
	UE_DEFINE_GAMEPLAY_TAG(Sound_Hit_RPR_Strong, "Sound.Hit.RPR.Strong");
	UE_DEFINE_GAMEPLAY_TAG(Sound_Hit_RPR_Guard, "Sound.Hit.RPR.Guard");

	UE_DEFINE_GAMEPLAY_TAG(Sound_Hit_SWD_Weak, "Sound.Hit.SWD.Weak");
	UE_DEFINE_GAMEPLAY_TAG(Sound_Hit_SWD_Strong, "Sound.Hit.SWD.Strong");
	UE_DEFINE_GAMEPLAY_TAG(Sound_Hit_SWD_Guard, "Sound.Hit.SWD.Guard");
#pragma endregion

#pragma region BossSound
	UE_DEFINE_GAMEPLAY_TAG(Sound_Boss_GuardBlock, "Sound.Boss.GuardBlock");
#pragma endregion

#pragma region GimmickSound
	UE_DEFINE_GAMEPLAY_TAG(Sound_Gimmick_Button_Press, "Sound.Gimmick.Button.Press");
	UE_DEFINE_GAMEPLAY_TAG(Sound_Gimmick_Pillar_Move, "Sound.Gimmick.Pillar.Move");
	UE_DEFINE_GAMEPLAY_TAG(Sound_Gimmick_Plate_Step, "Sound.Gimmick.Plate.Step");
	UE_DEFINE_GAMEPLAY_TAG(Sound_Gimmick_Door_Open, "Sound.Gimmick.Door.Open");
	UE_DEFINE_GAMEPLAY_TAG(Sound_Gimmick_Door_OpenComplete, "Sound.Gimmick.Door.OpenComplete");
	UE_DEFINE_GAMEPLAY_TAG(Sound_Gimmick_Boss000Pillar_Break, "Sound.Gimmick.Boss000Pillar.Break");
#pragma endregion

#pragma region UISound
	UE_DEFINE_GAMEPLAY_TAG(Sound_UI_Announce_BossCleared, "Sound.UI.Announce.BossCleared");
	UE_DEFINE_GAMEPLAY_TAG(Sound_UI_Announce_QuestCleared, "Sound.UI.Announce.QuestCleared");
#pragma endregion

#pragma region BGM
	UE_DEFINE_GAMEPLAY_TAG(Sound_BGM_Boss000_Battle, "Sound.BGM.Boss000.Battle");
	UE_DEFINE_GAMEPLAY_TAG(Sound_BGM_Boss000_Battle_Phase02, "Sound.BGM.Boss000.Battle.Phase02");
	UE_DEFINE_GAMEPLAY_TAG(Sound_BGM_Boss000_Battle_Fiver, "Sound.BGM.Boss000.Battle.Fiver");
	UE_DEFINE_GAMEPLAY_TAG(Sound_BGM_Boss000_Field, "Sound.BGM.Boss000.Field");
#pragma endregion
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
	UE_DEFINE_GAMEPLAY_TAG(Effect_Player_Wave03, "Effect.Player.Wave03");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Player_Wave04, "Effect.Player.Wave04");

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
