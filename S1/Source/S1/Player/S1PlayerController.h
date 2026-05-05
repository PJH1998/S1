// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "S1PlayerController.generated.h"

class US1InputData;

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

	void OnMove(const FInputActionValue& Value);
	void OnTurn(const FInputActionValue& Value);
};
