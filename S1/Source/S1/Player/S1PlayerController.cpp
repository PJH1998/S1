// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/S1PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "S1GameplayTags.h"
#include "System/S1AssetManager.h"
#include "Data/S1InputData.h"

AS1PlayerController::AS1PlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AS1PlayerController::BeginPlay()
{
	Super::BeginPlay();


	if (const US1InputData* InputData = US1AssetManager::GetAssetByTag<US1InputData>(S1AssetTags::Asset_InputData))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputData->InputMappingContext, 0);
		}
	}
}

void AS1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (const US1InputData * InputData = US1AssetManager::GetAssetByTag<US1InputData>(S1AssetTags::Asset_InputData))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

		if (const UInputAction* MoveAction = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_Move))
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::OnMove);
		}

		if (const UInputAction* TurnAction = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_Turn))
		{
			EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ThisClass::OnTurn);
		}
	}
}

void AS1PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void AS1PlayerController::OnMove(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	const FVector2D MoveVector = Value.Get<FVector2D>();

	UE_LOG(LogWindows, Log, TEXT("OnMove: %s"), *MoveVector.ToString());

	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	// X축: 좌우 (A/D), Y축: 전후 (W/S)
	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	ControlledPawn->AddMovementInput(ForwardDir, MoveVector.Y);
	ControlledPawn->AddMovementInput(RightDir,   MoveVector.X);
}

void AS1PlayerController::OnTurn(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	AddYawInput(LookVector.X);
	AddPitchInput(LookVector.Y);
}
