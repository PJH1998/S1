// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/S1MonsterHPBarComponent.h"

#include "Character/S1Monster.h"
#include "Data/S1UIData.h"
#include "System/S1AssetManager.h"
#include "Tags/S1GameplayTags.h"
#include "UI/ProgressBar/HPBar/S1MonsterHPBar.h"

US1MonsterHPBarComponent::US1MonsterHPBarComponent()
{
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawAtDesiredSize(true);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetGenerateOverlapEvents(false);
	SetHiddenInGame(true);
	SetRelativeLocation(HPBarRelativeLocation);
}

void US1MonsterHPBarComponent::ApplyWidgetSettings()
{
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawAtDesiredSize(true);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetGenerateOverlapEvents(false);
	SetHiddenInGame(true);
	SetRelativeLocation(HPBarRelativeLocation);
}

void US1MonsterHPBarComponent::ResetForPoolSpawn()
{
	if (MonsterHPBar != nullptr)
	{
		MonsterHPBar->SetMonster(OwnerMonster);
		MonsterHPBar->ForceHide();
	}

	SetHiddenInGame(true);
}

void US1MonsterHPBarComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerMonster = Cast<AS1Monster>(GetOwner());
	ApplyWidgetSettings();

	if (const US1UIData* UIData = US1AssetManager::GetAssetByTag<US1UIData>(S1AssetTags::Asset_UIData))
	{
		if (TSubclassOf<US1BaseWidget> MonsterHPBarWidgetClass = UIData->FindUserWidgetClassByTag(S1UITags::UI_Monster_HPBar))
		{
			SetWidgetClass(MonsterHPBarWidgetClass);
		}
	}

	if (AttachHPBarSocketName != NAME_None)
	{
		AttachToComponent(GetAttachParent(), FAttachmentTransformRules::KeepRelativeTransform, AttachHPBarSocketName);
	}

	InitWidget();
	InitializeWidget();

	if (OwnerMonster != nullptr)
	{
		OwnerMonster->OnHasTargetChanged.AddDynamic(this, &ThisClass::HandleHasTargetChanged);
		OwnerMonster->OnReturnedToPool.AddDynamic(this, &ThisClass::HandleReturnedToPool);
		OwnerMonster->OnMonsterNameChanged.AddDynamic(this, &ThisClass::HandleMonsterNameChanged);
		UpdateBarVisibility(OwnerMonster->HasTarget());
	}
}

void US1MonsterHPBarComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MonsterHPBar != nullptr)
	{
		MonsterHPBar->OnHideFinished.RemoveDynamic(this, &ThisClass::HandleHideAnimationFinished);
	}

	if (OwnerMonster != nullptr)
	{
		OwnerMonster->OnHasTargetChanged.RemoveDynamic(this, &ThisClass::HandleHasTargetChanged);
		OwnerMonster->OnReturnedToPool.RemoveDynamic(this, &ThisClass::HandleReturnedToPool);
		OwnerMonster->OnMonsterNameChanged.RemoveDynamic(this, &ThisClass::HandleMonsterNameChanged);
	}

	OwnerMonster = nullptr;
	MonsterHPBar = nullptr;

	Super::EndPlay(EndPlayReason);
}

void US1MonsterHPBarComponent::InitializeWidget()
{
	MonsterHPBar = Cast<US1MonsterHPBar>(GetWidget());
	if (MonsterHPBar == nullptr)
	{
		return;
	}

	MonsterHPBar->OnHideFinished.AddDynamic(this, &ThisClass::HandleHideAnimationFinished);
	MonsterHPBar->SetMonster(OwnerMonster);
	MonsterHPBar->ForceHide();
}

void US1MonsterHPBarComponent::UpdateBarVisibility(bool bInHasTarget)
{
	if (MonsterHPBar == nullptr || !IsValid(OwnerMonster) || OwnerMonster->IsDead())
	{
		SetHiddenInGame(true);
		if (MonsterHPBar != nullptr)
		{
			MonsterHPBar->ForceHide();
		}
		return;
	}

	if (bInHasTarget)
	{
		SetHiddenInGame(false);
		MonsterHPBar->ShowBar();
	}
	else
	{
		MonsterHPBar->HideBar();
	}
}

void US1MonsterHPBarComponent::HandleHasTargetChanged(AS1Monster* InMonster, bool bInHasTarget)
{
	if (InMonster != OwnerMonster)
	{
		return;
	}

	UpdateBarVisibility(bInHasTarget);
}

void US1MonsterHPBarComponent::HandleMonsterNameChanged(AS1Monster* InMonster)
{
	if (InMonster != OwnerMonster || MonsterHPBar == nullptr)
	{
		return;
	}

	MonsterHPBar->SetMonster(OwnerMonster);
}

void US1MonsterHPBarComponent::HandleReturnedToPool(AS1Monster* InMonster)
{
	if (InMonster != OwnerMonster)
	{
		return;
	}

	if (MonsterHPBar != nullptr)
	{
		MonsterHPBar->ForceHide();
	}

	SetHiddenInGame(true);
}

void US1MonsterHPBarComponent::HandleHideAnimationFinished()
{
	SetHiddenInGame(true);
}
