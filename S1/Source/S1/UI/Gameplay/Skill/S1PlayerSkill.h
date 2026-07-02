// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/S1BaseWidget.h"
#include "S1PlayerSkill.generated.h"

class US1SkillIcon;

/**
 * 
 */
UCLASS()
class S1_API US1PlayerSkill : public US1BaseWidget
{
	GENERATED_BODY()
	
public:
	US1PlayerSkill(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void InitializeSkillIcons();
	void InitializeSkillTextures();
	void ApplyCooldowns();

private:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<US1SkillIcon> SkillIcon_01;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<US1SkillIcon> SkillIcon_02;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<US1SkillIcon> SkillIcon_03;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<US1SkillIcon> SkillIcon_04;

	UPROPERTY(Transient)
	TArray<TObjectPtr<US1SkillIcon>> SkillIcons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	TArray<float> CooldownRemainingTimes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	TArray<float> CooldownDurations;
};
