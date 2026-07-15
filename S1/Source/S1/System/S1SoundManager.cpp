// Fill out your copyright notice in the Description page of Project Settings.

#include "System/S1SoundManager.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "System/S1AssetManager.h"
#include "Data/S1SoundData.h"
#include "Tags/Asset/S1GameplayTags_Asset.h"
#include "AbilitySystem/S1HitReactLibrary.h"

void US1SoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	BGMComponentA = NewObject<UAudioComponent>(this);
	BGMComponentA->bAutoActivate = false;
	BGMComponentA->bAutoDestroy = false;
	BGMComponentA->RegisterComponentWithWorld(GetWorld());

	BGMComponentB = NewObject<UAudioComponent>(this);
	BGMComponentB->bAutoActivate = false;
	BGMComponentB->bAutoDestroy = false;
	BGMComponentB->RegisterComponentWithWorld(GetWorld());
}

void US1SoundManager::Deinitialize()
{
	if (IsValid(BGMComponentA))
	{
		BGMComponentA->DestroyComponent();
	}

	if (IsValid(BGMComponentB))
	{
		BGMComponentB->DestroyComponent();
	}

	Super::Deinitialize();
}

void US1SoundManager::PlayBGM(USoundBase* NewBGM, float FadeDuration, float Volume)
{
	if (false == IsValid(NewBGM))
	{
		return;
	}

	UAudioComponent* CurrentComp = GetActiveComponent();
	if (IsValid(CurrentComp) && CurrentComp->Sound == NewBGM && CurrentComp->IsPlaying())
	{
		// 같은 곡이 이미 재생 중이어도 볼륨만은 즉시 반영
		CurrentComp->SetVolumeMultiplier(Volume);
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(FadeTransitionTimerHandle);

	if (false == IsValid(CurrentComp) || false == CurrentComp->IsPlaying())
	{
		StartFadeIn(NewBGM, FadeDuration, Volume);
		return;
	}

	// 기존 BGM을 완전히 죽인 뒤에야 새 BGM을 FadeIn — 겹치는 구간(크로스페이드) 없이 순차 전환.
	CurrentComp->FadeOut(FadeDuration, 0.f);

	TWeakObjectPtr<US1SoundManager> WeakThis(this);
	TWeakObjectPtr<USoundBase> WeakBGM(NewBGM);
	FTimerDelegate FadeInDelegate = FTimerDelegate::CreateLambda([WeakThis, WeakBGM, FadeDuration, Volume]()
		{
			if (WeakThis.IsValid() && WeakBGM.IsValid())
			{
				WeakThis->StartFadeIn(WeakBGM.Get(), FadeDuration, Volume);
			}
		});
	GetWorld()->GetTimerManager().SetTimer(FadeTransitionTimerHandle, FadeInDelegate, FadeDuration, false);
}

void US1SoundManager::StopBGM(float FadeOutDuration)
{
	GetWorld()->GetTimerManager().ClearTimer(FadeTransitionTimerHandle);

	UAudioComponent* CurrentComp = GetActiveComponent();
	if (IsValid(CurrentComp) && CurrentComp->IsPlaying())
	{
		CurrentComp->FadeOut(FadeOutDuration, 0.f);
	}
}

void US1SoundManager::PlaySound(USoundBase* Sound)
{
	if (false == IsValid(Sound))
	{
		return;
	}

	UGameplayStatics::PlaySound2D(this, Sound);
}

void US1SoundManager::PlayHitSound(const FGameplayTag& BaseTag, ES1HitReactType HitType)
{
	const FGameplayTag OutcomeTag = S1HitReactLibrary::HitReactTypeToTag(HitType);
	const FGameplayTag ComposedTag = ComposeHitSoundTag(BaseTag, OutcomeTag);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(
			TEXT("[SoundManager] BaseTag: %s + OutcomeTag: %s -> ComposedTag: %s"),
			*BaseTag.ToString(), *OutcomeTag.ToString(),
			ComposedTag.IsValid() ? *ComposedTag.ToString() : TEXT("INVALID (태그 미선언?)")));
	}

	if (false == ComposedTag.IsValid())
	{
		return;
	}

	US1SoundData* SoundData = US1AssetManager::GetAssetByTag<US1SoundData>(S1AssetTags::Asset_SoundData);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(
			TEXT("[SoundManager] SoundData: %s, Sound: %s"),
			IsValid(SoundData) ? TEXT("Valid") : TEXT("nullptr (Asset_SoundData 등록 안 됨?)"),
			(IsValid(SoundData) && SoundData->FindSound(ComposedTag)) ? TEXT("Found") : TEXT("nullptr (Map에 없음)")));
	}

	if (false == IsValid(SoundData))
	{
		return;
	}

	PlaySound(SoundData->FindSound(ComposedTag));
}

FGameplayTag US1SoundManager::ComposeHitSoundTag(const FGameplayTag& BaseTag, const FGameplayTag& OutcomeTag) const
{
	if (false == BaseTag.IsValid() || false == OutcomeTag.IsValid())
	{
		return FGameplayTag::EmptyTag;
	}

	// OutcomeTag는 "HitType.Strong"처럼 여러 세그먼트일 수 있어 마지막 세그먼트만 BaseTag 뒤에 붙인다.
	FString OutcomeLeaf;
	if (false == OutcomeTag.ToString().Split(TEXT("."), nullptr, &OutcomeLeaf, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
	{
		OutcomeLeaf = OutcomeTag.ToString();
	}

	const FString ComposedName = BaseTag.ToString() + TEXT(".") + OutcomeLeaf;
	return FGameplayTag::RequestGameplayTag(FName(*ComposedName), /*ErrorIfNotFound=*/false);
}

void US1SoundManager::StartFadeIn(USoundBase* NewBGM, float FadeDuration, float Volume)
{
	UAudioComponent* NextComp = GetInactiveComponent();
	if (false == IsValid(NextComp))
	{
		return;
	}

	NextComp->SetSound(NewBGM);
	NextComp->FadeIn(FadeDuration, Volume);

	bIsAActive = !bIsAActive;
}

UAudioComponent* US1SoundManager::GetActiveComponent() const
{
	return bIsAActive ? BGMComponentA : BGMComponentB;
}

UAudioComponent* US1SoundManager::GetInactiveComponent() const
{
	return bIsAActive ? BGMComponentB : BGMComponentA;
}
