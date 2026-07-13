// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TimerManager.h"
#include "GameplayTagContainer.h"
#include "S1SoundManager.generated.h"

class UAudioComponent;
class USoundBase;

// BGM(루프, FadeOut 완료 후 FadeIn 순차 전환) + 1회성 사운드 재생을 담당하는 월드 서브시스템.
// 데디케이트 서버에는 들려줄 대상이 없어 BGM 컴포넌트를 아예 생성하지 않음.
UCLASS()
class S1_API US1SoundManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void PlayBGM(USoundBase* NewBGM, float FadeDuration);

	// 위치 없는 1회성 재생 — 호출부가 이미 재생할 Sound를 들고 있을 때 사용 (예: UI).
	void PlaySound(USoundBase* Sound);

	// BaseTag(무기별 사운드 카테고리, 예: Sound.Hit.RPR) + OutcomeTag(HitType_Weak/Strong/ToAir 등)를
	// 조합해 미리 선언된 태그(예: Sound.Hit.RPR.Strong)로 US1HitSoundData를 조회 후 재생.
	void PlayHitSound(const FGameplayTag& BaseTag, const FGameplayTag& OutcomeTag);

private:
	void StartFadeIn(USoundBase* NewBGM, float FadeDuration);
	FGameplayTag ComposeHitSoundTag(const FGameplayTag& BaseTag, const FGameplayTag& OutcomeTag) const;

	UAudioComponent* GetActiveComponent() const;
	UAudioComponent* GetInactiveComponent() const;

private:
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> BGMComponentA;

	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> BGMComponentB;

	bool bIsAActive = true;

	FTimerHandle FadeTransitionTimerHandle;
};
