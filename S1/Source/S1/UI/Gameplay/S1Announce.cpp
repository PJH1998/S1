// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/S1Announce.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Data/S1UIResource.h"
#include "System/S1AssetManager.h"
#include "System/S1SoundManager.h"
#include "Tags/S1GameplayTags.h"

US1Announce::US1Announce(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1Announce::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);

	if (Anim_FadeOut)
	{
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &ThisClass::HandleHideAnimationFinished);
		BindToAnimationFinished(Anim_FadeOut, EndEvent);
	}
}

void US1Announce::ShowAnnounce(const FGameplayTag& AnnounceTag)
{
	if (Image_Announce == nullptr)
	{
		return;
	}

	const US1UIResource* UIResource = US1AssetManager::GetAssetByTag<US1UIResource>(S1AssetTags::Asset_UIResource);
	UTexture2D* Texture = UIResource ? UIResource->FindTextureByTag(AnnounceTag) : nullptr;
	if (Texture == nullptr)
	{
		return;
	}

	// 팡파레 — UI.Icon.Announce.<Leaf> → Sound.UI.Announce.<Leaf> 조합 조회(미선언이면 무음).
	// 위젯은 클라에만 존재하므로 별도 데디서버 가드 불필요.
	FString AnnounceLeaf;
	if (AnnounceTag.ToString().Split(TEXT("."), nullptr, &AnnounceLeaf, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
	{
		const FGameplayTag FanfareTag = FGameplayTag::RequestGameplayTag(
			FName(TEXT("Sound.UI.Announce.") + AnnounceLeaf), /*ErrorIfNotFound=*/false);
		if (FanfareTag.IsValid())
		{
			if (US1SoundManager* SoundManager = GetWorld()->GetSubsystem<US1SoundManager>())
			{
				SoundManager->PlaySoundByTag(FanfareTag);
			}
		}
	}

	Image_Announce->SetBrushFromTexture(Texture);
	SetVisibility(ESlateVisibility::HitTestInvisible);
	if (Anim_FadeIn)
	{
		PlayAnimation(Anim_FadeIn);
	}

	GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &ThisClass::Hide, DisplayDuration, false);
}

void US1Announce::Hide()
{
	if (Anim_FadeOut)
	{
		PlayAnimation(Anim_FadeOut);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void US1Announce::HandleHideAnimationFinished()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
