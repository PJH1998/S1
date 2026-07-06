// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/Skill/S1PlayerSkill.h"

#include "AbilitySystem/AbilitySystemComponent/Player/S1PlayerAbilitySystemComponent.h"
#include "Data/S1CharacterSelectData.h"
#include "Data/S1UIResource.h"
#include "Player/S1PlayerState.h"
#include "Tags/S1GameplayTags.h"
#include "System/S1AssetManager.h"
#include "UI/Gameplay/Skill/S1SkillIcon.h"

US1PlayerSkill::US1PlayerSkill(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CooldownRemainingTimes.Init(0.f, 4);
	CooldownDurations.Init(0.f, 4);
}

void US1PlayerSkill::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeSkillIcons();
	InitializeSkillTextures();
	ApplyCooldowns();
}

void US1PlayerSkill::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (false == bSkillTexturesInitialized)
	{
		InitializeSkillTextures();
	}

	AS1PlayerState* PS = GetOwningPlayerState<AS1PlayerState>();
	US1PlayerAbilitySystemComponent* ASC = PS ? Cast<US1PlayerAbilitySystemComponent>(PS->GetS1AbilitySystemComponent()) : nullptr;
	if (ASC == nullptr)
	{
		return;
	}

	static const FGameplayTag SkillTags[3] =
	{
		S1AbilityTags::Ability_Player_Attack_Skill01,
		S1AbilityTags::Ability_Player_Attack_Skill02,
		S1AbilityTags::Ability_Player_Attack_Skill03,
	};

	for (int32 Index = 0; Index < 3; ++Index)
	{
		if (!CooldownRemainingTimes.IsValidIndex(Index) || !CooldownDurations.IsValidIndex(Index))
		{
			continue;
		}

		float Remaining = 0.f;
		float Duration = 0.f;
		ASC->GetSkillCooldown(SkillTags[Index], Remaining, Duration);

		CooldownRemainingTimes[Index] = Remaining;
		CooldownDurations[Index] = Duration;
	}

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
	AS1PlayerState* PS = GetOwningPlayerState<AS1PlayerState>();
	const FGameplayTag SelectedCharacterTag = PS ? PS->GetSelectedCharacterTag() : FGameplayTag();
	if (false == SelectedCharacterTag.IsValid())
	{
		return;
	}

	const US1CharacterSelectData* CharacterSelectData = US1AssetManager::GetAssetByTag<US1CharacterSelectData>(S1AssetTags::Asset_CharacterData);
	const FS1SelectCharacterEntry* Entry = CharacterSelectData ? CharacterSelectData->FindEntryByTag(SelectedCharacterTag) : nullptr;
	if (Entry == nullptr)
	{
		return;
	}

	const US1UIResource* UIResource = US1AssetManager::GetAssetByTag<US1UIResource>(S1AssetTags::Asset_UIResource);
	if (UIResource == nullptr)
	{
		return;
	}

	const TArray<FGameplayTag> SkillTextureTags =
	{
		S1UIResourceTags::UI_Tex_Skill01,
		S1UIResourceTags::UI_Tex_Skill02,
		S1UIResourceTags::UI_Tex_Skill03,
		S1UIResourceTags::UI_Tex_Heal,
	};

	for (int32 Index = 0; Index < SkillIcons.Num() && Index < SkillTextureTags.Num(); ++Index)
	{
		if (US1SkillIcon* SkillIcon = SkillIcons[Index])
		{
			const FGameplayTag GenderedTag = US1BaseWidget::AppendGenderSuffix(SkillTextureTags[Index], Entry->Gender);
			SkillIcon->SetIconTexture(UIResource->FindTextureByTag(GenderedTag));
		}
	}

	bSkillTexturesInitialized = true;
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
