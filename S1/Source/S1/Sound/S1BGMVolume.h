// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "S1BGMVolume.generated.h"

class UBoxComponent;
class USoundBase;

// 레벨에 배치하는 BGM 구역 볼륨. 로컬 플레이어 폰이 진입하면 US1SoundManager에 재생을 요청한다.
// 구역끼리 겹치지 않는다는 전제로 동작.
// 플레이어/컨트롤러 쪽 코드 변경 없이 스폰 시점(이미 겹친 상태)까지 자체 처리 — BeginPlay 직후
// 로컬 폰을 찾을 때까지만 짧게 Tick으로 직접 위치를 확인하고, 찾으면 즉시 Tick을 끈다.
UCLASS()
class S1_API AS1BGMVolume : public AActor
{
	GENERATED_BODY()

public:
	AS1BGMVolume();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnZoneEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	void RequestPlayBGM();
	bool IsLocationInsideZone(const FVector& Location) const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BGM", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> ZoneVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> ZoneBGM;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM", meta = (AllowPrivateAccess = "true"))
	float FadeDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM", meta = (AllowPrivateAccess = "true"))
	float Volume = 1.0f;

	// 구역 경계에서 왕복 시 재생이 반복 전환되는 걸 막기 위한 디바운스 — 이 시간 이상 머물러야 실제로 전환됨
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM", meta = (AllowPrivateAccess = "true"))
	float DebounceDuration = 1.0f;

	FTimerHandle DebounceTimerHandle;
};
