// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/Skill/S1SkillIcon.h"

#include "Components/Image.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"

US1SkillIcon::US1SkillIcon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1SkillIcon::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeCooldownMaterial();
	UpdateCooldownVisual();
}

void US1SkillIcon::SetIconTexture(UTexture2D* InTexture)
{
	if (Image_BG == nullptr || InTexture == nullptr)
	{
		return;
	}

	IconTexture = InTexture;
	if (CooldownMaskMID)
	{
		CooldownMaskMID->SetTextureParameterValue(IconTextureParameterName, IconTexture);
	}
}

void US1SkillIcon::SetCooldown(float InRemainingTime, float InDuration)
{
	CooldownDuration = FMath::Max(InDuration, 0.f);
	CooldownRemainingTime = CooldownDuration > 0.f ? FMath::Clamp(InRemainingTime, 0.f, CooldownDuration) : 0.f;

	UpdateCooldownVisual();
	SetUsable(CooldownRemainingTime <= 0.f);
}

void US1SkillIcon::SetUsable(bool bInUsable)
{
	if (Image_Frame)
	{
		Image_Frame->SetVisibility(bInUsable ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void US1SkillIcon::InitializeCooldownMaterial()
{
	if (Image_BG == nullptr || CooldownMaskMaterial == nullptr)
	{
		return;
	}

	CooldownMaskMID = UMaterialInstanceDynamic::Create(CooldownMaskMaterial, this);
	if (CooldownMaskMID == nullptr)
	{
		UE_LOG(LogWindows, Error, TEXT("Failed to Create : CooldownMask MID"));
		return;
	}

	Image_BG->SetBrushFromMaterial(CooldownMaskMID);

	if (IconTexture)
	{
		CooldownMaskMID->SetTextureParameterValue(IconTextureParameterName, IconTexture);
	}
}

void US1SkillIcon::UpdateCooldownVisual()
{
	const float CooldownRatio = CooldownDuration > KINDA_SMALL_NUMBER ? CooldownRemainingTime / CooldownDuration : 0.f;

	if (CooldownMaskMID)
	{
		CooldownMaskMID->SetScalarParameterValue(CooldownRatioParameterName, CooldownRatio);
	}
}
