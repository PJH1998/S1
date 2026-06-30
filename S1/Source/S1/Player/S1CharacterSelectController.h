// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "S1CharacterSelectController.generated.h"

class AS1Player;
class US1CharacterSelectData;

UCLASS()
class S1_API AS1CharacterSelectController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void OnSelectA();
	void OnSelectB();
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

	//// 미리보기 액터가 배치될 트랜스폼 (에디터에서 설정)
	//UPROPERTY(EditDefaultsOnly, Category = "CharacterSelect")
	//FTransform PreviewTransform;

private:
	UPROPERTY()
	TObjectPtr<AS1Player> PreviewActor;

	UPROPERTY()
	TObjectPtr<US1CharacterSelectData> SelectData;

	FGameplayTag CurrentSelectedTag;

	FTransform PreviewTransform;
};
