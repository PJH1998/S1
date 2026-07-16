// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TimerManager.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
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

	void PlayBGM(USoundBase* NewBGM, float FadeDuration = 0.f, float Volume = 1.f);

	// 새 BGM 없이 현재 재생 중인 BGM만 FadeOut — 전환 종료 시점을 별도로 제어하고 싶을 때(예: UI 연출과 동기화) 사용.
	void StopBGM(float FadeOutDuration = 0.f);

	// 위치 없는 1회성 재생 — 호출부가 이미 재생할 Sound를 들고 있을 때 사용 (예: UI).
	void PlaySound(USoundBase* Sound);

	// BaseTag(무기 카테고리: Sword/Fist/Bite 등) + HitType(Weak/Strong/ToAir)을 내부에서 태그로 조합해
	// US1SoundData를 조회 후 재생.
	void PlayHitSound(const FGameplayTag& BaseTag, ES1HitReactType HitType);

private:
	void StartFadeIn(USoundBase* NewBGM, float FadeDuration, float Volume);
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
