// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "S1CharacterSelectComponent.generated.h"

class AS1SelectCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class S1_API US1CharacterSelectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	US1CharacterSelectComponent();

protected:
	virtual void BeginPlay() override;

public:
	//TEST — UI 슬롯 클릭에서도 호출
	void OnSelectA();
	void OnSelectB();
	void OnWeaponSelect(const FGameplayTag& WeaponTag);
	void RequestConfirm();

	// AS1PlayerController::AcknowledgePossession에서 실제 게임플레이 Pawn으로 교체된 직후 호출 — 로컬 프리뷰 정리 + UI 전환
	void HandleGameplayPawnPossessed();

	AS1SelectCharacter* GetPreviewActor() const { return PreviewActor; }

private:
	void OnConfirm();

	void SelectCharacter(const FGameplayTag& Tag);
	void UpdatePreview(const FGameplayTag& Tag);

	// 로컬 클라이언트 전용 부스 프리뷰 액터 스폰 — 서버에 알리지 않는 순수 코스메틱 액터
	void SpawnLocalPreview();

	UFUNCTION(Server, Reliable)
	void ServerSetSelectedCharacter(FGameplayTag Tag);

	UFUNCTION(Server, Reliable)
	void ServerSetSelectedWeapon(FGameplayTag Tag);

	// 확정 → 서버 권위에서 실제 게임플레이 Pawn을 스폰하고 in-place로 Possess
	UFUNCTION(Server, Reliable)
	void ServerConfirmSelection();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "CharacterSelect")
	FGameplayTag CharacterTagA;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterSelect")
	FGameplayTag CharacterTagB;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterSelect")
	TSubclassOf<AS1SelectCharacter> PreviewCharacterClass;

private:
	UPROPERTY()
	TObjectPtr<AS1SelectCharacter> PreviewActor;

	FGameplayTag CurrentSelectedTag;

	// HandleGameplayPawnPossessed 중복 실행 방지(리슨 서버 로컬 플레이어는 OnPossess+AcknowledgePossession 둘 다 호출됨)
	bool bGameplayUISwapped = false;
};
