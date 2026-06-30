// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "S1CharacterSelectController.generated.h"

class AS1SelectCharacter;

UCLASS()
class S1_API AS1CharacterSelectController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	//TEST
	void OnSelectA();
	void OnSelectB();
	void OnWeaponSelectA();
	void OnWeaponSelectB();
	void OnConfirm();

	void SelectCharacter(const FGameplayTag& Tag);
	void UpdatePreview(const FGameplayTag& Tag);

	UFUNCTION(Server, Reliable)
	void ServerSetSelectedCharacter(FGameplayTag Tag);

	// 확정 → 서버 권위에서 seamless travel 시작
	UFUNCTION(Server, Reliable)
	void ServerConfirmSelection();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "CharacterSelect")
	FGameplayTag CharacterTagA;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterSelect")
	FGameplayTag CharacterTagB;

private:
	UPROPERTY()
	TObjectPtr<AS1SelectCharacter> PreviewActor;

	FGameplayTag CurrentSelectedTag;
};
