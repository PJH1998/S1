// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "S1PlayerController.generated.h"

class AS1Player;

UCLASS()
class S1_API AS1PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AS1PlayerController(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

public:
	virtual void HandleGameplayEvent(FGameplayTag EventTag);

private:
	void OnMove(const FInputActionValue& Value);
	void OnTurn(const FInputActionValue& Value);
	void OnAttack(const FInputActionValue& Value);
	void OnSprint(const FInputActionValue& Value);
	void OnSkill01(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AS1Player> S1Player;
};
