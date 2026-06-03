// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/S1PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "S1GameplayTags.h"
#include "System/S1AssetManager.h"
#include "Data/S1InputData.h"
#include "Character/Player/S1Player.h"
#include "Component/S1LockOnComponent.h"

#include "System/S1UIManager.h"

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

	if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
	{
		UIManager->Create_RootUI(this);
	}
	S1Player = Cast<AS1Player>(GetCharacter());
}

void AS1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (const US1InputData * InputData = US1AssetManager::GetAssetByTag<US1InputData>(S1AssetTags::Asset_InputData))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);


#pragma region Move
		if (const UInputAction* MoveAction = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_Move))
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::OnMove);
		}

		if (const UInputAction* TurnAction = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_Turn))
		{
			EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ThisClass::OnTurn);
		}
		
		if (const UInputAction* SprintAction = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_Sprint))
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ThisClass::OnSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::OnSprint);
		}
#pragma endregion

#pragma region Attack
		if (const UInputAction* AttackAction = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_Attack))
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::OnAttack);
		}

		if (const UInputAction* Skill01Action = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_Skill01))
		{
			EnhancedInputComponent->BindAction(Skill01Action, ETriggerEvent::Started, this, &ThisClass::OnSkill01);
		}
#pragma endregion

#pragma region Jump
		if (const UInputAction* JumpAction = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_Jump))
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::OnJump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::OnJump);
		}
#pragma endregion

#pragma region LockOn
		if (const UInputAction* LockOnAction = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_LockOn))
		{
			EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this, &ThisClass::OnLockOn);
		}
		if (const UInputAction* ChangeSideAction = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_ChangeLockOnSide))
		{
			EnhancedInputComponent->BindAction(ChangeSideAction, ETriggerEvent::Started, this, &ThisClass::OnChangeLockOnSide);
		}
		if (const UInputAction* CycleLeftAction = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_LockOnLeft))
		{
			EnhancedInputComponent->BindAction(CycleLeftAction, ETriggerEvent::Started, this, &ThisClass::OnCycleLeft);
		}
		if (const UInputAction* CycleRightAction = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_LockOnRight))
		{
			EnhancedInputComponent->BindAction(CycleRightAction, ETriggerEvent::Started, this, &ThisClass::OnCycleRight);
		}
#pragma endregion

	}
}

void AS1PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (false == IsValid(S1Player)) { return; }

	const US1LockOnComponent* LockOnComp = S1Player->GetLockOnComponent();
	if (false == (LockOnComp && LockOnComp->IsLockedOn())) { return; }

	const FVector ToTarget = (LockOnComp->GetLockOnTargetLocation()
		- S1Player->GetActorLocation()).GetSafeNormal2D();
	if (ToTarget.IsNearlyZero()) { return; }

	SetControlRotation(FRotator(GetControlRotation().Pitch, ToTarget.Rotation().Yaw, 0.f));
}

void AS1PlayerController::HandleGameplayEvent(FGameplayTag EventTag)
{
}

void AS1PlayerController::OnMove(const FInputActionValue& Value)
{
	if (false == IsValid(S1Player))
	{
		return;
	}

	const FVector2D MoveVector = Value.Get<FVector2D>();

	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	S1Player->AddMovementInput(ForwardDir, MoveVector.Y);
	S1Player->AddMovementInput(RightDir,   MoveVector.X);
}

void AS1PlayerController::OnTurn(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	AddYawInput(LookVector.X);
	AddPitchInput(LookVector.Y);
}

void AS1PlayerController::OnAttack(const FInputActionValue& Value)
{
	S1Player->ActivateAbility(S1AbilityTags::Ability_Player_Attack_WeakAttack);
}

void AS1PlayerController::OnSprint(const FInputActionValue& Value)
{
	if (false == IsValid(S1Player))
	{
		return;
	}

	S1Player->SetSprinting(Value.Get<bool>());
}

void AS1PlayerController::OnSkill01(const FInputActionValue& Value)
{
	S1Player->ActivateAbility(S1AbilityTags::Ability_Player_Attack_Skill01);
}

void AS1PlayerController::OnJump(const FInputActionValue& Value)
{
	if (false == IsValid(S1Player))
	{
		return;
	}

	if (Value.Get<bool>())
	{
		S1Player->Jump();
	}
	else
	{
		S1Player->StopJumping();
	}
}

void AS1PlayerController::OnLockOn(const FInputActionValue& Value)
{
	if (false == ::IsValid(S1Player)) { return; }

	if (US1LockOnComponent* Comp = S1Player->GetLockOnComponent())
	{
		FVector   CamLoc;
		FRotator  CamRot;
		GetPlayerViewPoint(CamLoc, CamRot);
		Comp->OnLockOnInput(CamRot);
	}
}

void AS1PlayerController::OnCycleLeft(const FInputActionValue& Value)
{
	if (false == ::IsValid(S1Player)) { return; }

	if (US1LockOnComponent* Comp = S1Player->GetLockOnComponent())
	{
		FVector  CamLoc;
		FRotator CamRot;
		GetPlayerViewPoint(CamLoc, CamRot);
		Comp->CycleLeft(CamRot);
	}
}

void AS1PlayerController::OnCycleRight(const FInputActionValue& Value)
{
	if (false == ::IsValid(S1Player)) { return; }

	if (US1LockOnComponent* Comp = S1Player->GetLockOnComponent())
	{
		FVector  CamLoc;
		FRotator CamRot;
		GetPlayerViewPoint(CamLoc, CamRot);
		Comp->CycleRight(CamRot);
	}
}

void AS1PlayerController::OnChangeLockOnSide(const FInputActionValue& Value)
{
	if (false == ::IsValid(S1Player)) { return; }

	if (US1LockOnComponent* Comp = S1Player->GetLockOnComponent())
	{
		Comp->ToggleCameraSide();
	}
}
