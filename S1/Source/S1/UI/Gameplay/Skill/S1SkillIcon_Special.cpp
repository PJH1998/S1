// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/Skill/S1SkillIcon_Special.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Data/S1CharacterSelectData.h"
#include "Data/S1UIResource.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Player/S1PlayerState.h"
#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "System/S1AssetManager.h"
#include "Tags/S1GameplayTags.h"

void US1SkillIcon_Special::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeGaugeMaterial();

	if (AS1PlayerState* PS = Cast<AS1PlayerState>(GetOwningPlayerState()))
	{
		PlayerSet = PS->GetS1PlayerSet();
	}

	InitializeIconTexture();
}

void US1SkillIcon_Special::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// PlayerState 복제가 늦게 도착할 수 있으므로 null이면 재조회
	if (PlayerSet == nullptr)
	{
		if (AS1PlayerState* PS = Cast<AS1PlayerState>(GetOwningPlayerState()))
		{
			PlayerSet = PS->GetS1PlayerSet();
		}

		if (PlayerSet == nullptr)
		{
			return;
		}
	}

	if (false == bIconTextureInitialized)
	{
		InitializeIconTexture();
	}

	//CurrentGaugeValue = PlayerSet->GetCurrentUltimateGauge();
	//MaxGaugeValue = PlayerSet->GetMaxUltimateGauge();
	CurrentGaugeValue += InDeltaTime * 10.f;
	MaxGaugeValue = 100.f;

	if (LerpGaugeValue <= CurrentGaugeValue)
	{
		const float LerpDelta = FMath::Max(MaxGaugeValue, 1.f) * LerpSpeed * InDeltaTime;
		LerpGaugeValue = FMath::Min(CurrentGaugeValue, LerpGaugeValue + LerpDelta);
	}
	else
	{
		LerpGaugeValue = CurrentGaugeValue;
	}

	UpdateGaugeVisual();
}

void US1SkillIcon_Special::InitializeGaugeMaterial()
{
	if (Image_Gauge == nullptr || GaugeMaterial == nullptr)
	{
		return;
	}

	GaugeMID = UMaterialInstanceDynamic::Create(GaugeMaterial, this);
	if (GaugeMID == nullptr)
	{
		UE_LOG(LogWindows, Error, TEXT("Failed to Create : Gauge MID"));
		return;
	}

	Image_Gauge->SetBrushFromMaterial(GaugeMID);
}

void US1SkillIcon_Special::InitializeIconTexture()
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

	const FGameplayTag GenderedTag = US1BaseWidget::AppendGenderSuffix(S1UIResourceTags::UI_Tex_Ultimate, Entry->Gender);
	UTexture2D* IconTexture = UIResource->FindTextureByTag(GenderedTag);
	if (IconTexture == nullptr || Image_Icon == nullptr)
	{
		return;
	}

	Image_Icon->SetBrushFromTexture(IconTexture);
	bIconTextureInitialized = true;
}

void US1SkillIcon_Special::UpdateGaugeVisual()
{
	const float FillRatio = MaxGaugeValue > KINDA_SMALL_NUMBER ? FMath::Clamp(CurrentGaugeValue / MaxGaugeValue, 0.f, 1.f) : 0.f;
	const float LerpRatio = MaxGaugeValue > KINDA_SMALL_NUMBER ? FMath::Clamp(LerpGaugeValue / MaxGaugeValue, 0.f, 1.f) : 0.f;

	if (GaugeMID)
	{
		GaugeMID->SetScalarParameterValue(FillRatioParameterName, FillRatio);
		GaugeMID->SetScalarParameterValue(LerpRatioParameterName, LerpRatio);
	}

	const bool bIsGaugeFull = FillRatio >= 1.f;

	if (Image_Screen)
	{
		Image_Screen->SetVisibility(bIsGaugeFull ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}

	if (Image_Effect)
	{
		Image_Effect->SetVisibility(bIsGaugeFull ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}

	if (bIsGaugeFull && false == bWasGaugeFull && Anim_Effect_Flash)
	{
		PlayAnimation(Anim_Effect_Flash);
	}

	bWasGaugeFull = bIsGaugeFull;
}

