// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/Skill/S1PlayerSkill.h"

#include "Data/S1UIResource.h"
#include "Tags/S1GameplayTags.h"
#include "System/S1AssetManager.h"
#include "UI/Gameplay/Skill/S1SkillIcon.h"

US1PlayerSkill::US1PlayerSkill(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Temp
	CooldownRemainingTimes.Init(5.f, 4);
	CooldownDurations.Init(10.f, 4);
}

void US1PlayerSkill::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeSkillIcons();
	InitializeSkillTextures();
	ApplyCooldowns();
}

void US1PlayerSkill::InitializeSkillIcons()
{
	SkillIcons.Reset();
	SkillIcons.Add(SkillIcon_01);
	SkillIcons.Add(SkillIcon_02);
	SkillIcons.Add(SkillIcon_03);
	SkillIcons.Add(SkillIcon_04);
}

void US1PlayerSkill::InitializeSkillTextures()
{
	const US1UIResource* UIResource = US1AssetManager::GetAssetByTag<US1UIResource>(S1AssetTags::Asset_UIResource);
	if (UIResource == nullptr)
	{
		return;
	}

	const TArray<FGameplayTag> SkillTextureTags =
	{
		S1UIResourceTags::UI_Tex_Asna_Skill01,
		S1UIResourceTags::UI_Tex_Asna_Skill02,
		S1UIResourceTags::UI_Tex_Asna_Skill03,
		S1UIResourceTags::UI_Tex_Heal,
	};

	for (int32 Index = 0; Index < SkillIcons.Num() && Index < SkillTextureTags.Num(); ++Index)
	{
		if (US1SkillIcon* SkillIcon = SkillIcons[Index])
		{
			SkillIcon->SetIconTexture(UIResource->FindTextureByTag(SkillTextureTags[Index]));
		}
	}
}

void US1PlayerSkill::ApplyCooldowns()
{
	for (int32 Index = 0; Index < SkillIcons.Num(); ++Index)
	{
		if (US1SkillIcon* SkillIcon = SkillIcons[Index])
		{
			const float RemainingTime = CooldownRemainingTimes.IsValidIndex(Index) ? CooldownRemainingTimes[Index] : 0.f;
			const float Duration = CooldownDurations.IsValidIndex(Index) ? CooldownDurations[Index] : 0.f;
			SkillIcon->SetCooldown(RemainingTime, Duration);
		}
	}
}
