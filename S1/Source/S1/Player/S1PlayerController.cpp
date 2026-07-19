// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/S1PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/GameViewportClient.h"

#include "Tags/S1GameplayTags.h"
#include "System/S1AssetManager.h"
#include "Data/S1InputData.h"
#include "Character/Player/S1Player.h"
#include "Component/S1CharacterSelectComponent.h"
#include "Component/S1LockOnComponent.h"

#include "System/S1GameInstance.h"
#include "System/S1UIManager.h"
#include "UI/Menu/S1Inventory_ItemInfo.h"
#include "UI/S1RootWidget.h"
#include "S1LogChannels.h"

AS1PlayerController::AS1PlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CharacterSelectComponent = CreateDefaultSubobject<US1CharacterSelectComponent>(TEXT("CharacterSelectComponent"));
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

	// Server일 때 UI 생성 가드
	if (!IsLocalController() || GetNetMode() == NM_DedicatedServer) return;

	// Root 위젯은 세션 내내 UI_Root_Gameplay 하나만 사용 — 부스/게임플레이 전환은 RootWidget을 갈아치우는 게 아니라
	// 그 안의 HUD 패널 콘텐츠만 SetUp_Panel로 교체(Lobby_HUD → Gameplay_HUD, CharacterSelectComponent::HandleGameplayPawnPossessed)
	US1RootWidget* Root = nullptr;
	if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
	{
		UIManager->Create_RootUI(this, S1UITags::UI_Root_Gameplay);
		Root = UIManager->GetRootWidget();

		if (::IsValid(Root))
		{
			Root->SetCursorVisible(true);
			Root->SetUp_Panel(UI_TYPE::HUD, S1UITags::UI_Lobby_HUD);
		}
	}

	// WidgetToFocus를 안 주면 뷰포트/윈도우가 아직 포커스를 안 가진 상태라 첫 클릭이 UI에 전달 안 되고
	// 포커스 획득에만 소모됨(버튼 OnClicked가 씹히고 두 번째 클릭에야 동작하던 원인) — 위젯 생성 후 명시적으로 포커스 지정
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	if (::IsValid(Root))
	{
		InputMode.SetWidgetToFocus(Root->TakeWidget());
	}
	SetInputMode(InputMode);

	if (US1GameInstance* GI = GetGameInstance<US1GameInstance>())
	{
		GI->HideBootstrapOverlay();
	}
}

void AS1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//TEST — 부스 캐릭터 선택 키 바인딩
	if (US1CharacterSelectComponent* Comp = CharacterSelectComponent.Get())
	{
		InputComponent->BindKey(EKeys::NumPadOne, IE_Pressed, Comp, &US1CharacterSelectComponent::OnSelectA);
		InputComponent->BindKey(EKeys::NumPadTwo, IE_Pressed, Comp, &US1CharacterSelectComponent::OnSelectB);
		InputComponent->BindKey(EKeys::NumPadThree, IE_Pressed, Comp, &US1CharacterSelectComponent::RequestConfirm);
	}

	if (const US1InputData * InputData = US1AssetManager::GetAssetByTag<US1InputData>(S1AssetTags::Asset_InputData))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

#pragma region Move
		if (const UInputAction* MoveAction = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_Move))
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,  this, &ThisClass::OnMove);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed,  this, &ThisClass::OnMoveReleased);
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

#pragma region Interact
		if (const UInputAction* InteractAction = InputData->FindInputActionByTag(S1GameplayTags::Input_Action_Interact))
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::OnInteract);
		}
#pragma endregion

#pragma region UI
		if (const UInputAction* InventoryAction = InputData->FindInputActionByTag(S1GameplayTags::Input_UI_Inventory))
		{
			EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &ThisClass::OnInventory);
		}

		if (const UInputAction* FreeCursorAction = InputData->FindInputActionByTag(S1GameplayTags::Input_UI_FreeCursor))
		{
			EnhancedInputComponent->BindAction(FreeCursorAction, ETriggerEvent::Started, this, &ThisClass::OnFreeCursorPressed);
			EnhancedInputComponent->BindAction(FreeCursorAction, ETriggerEvent::Completed, this, &ThisClass::OnFreeCursorReleased);
		}
#pragma endregion
	}
}

void AS1PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (false == IsValid(S1Player)) { return; }

	// [TEST] Q: SWD01으로 무기 교체 / E: 기본 무기로 복귀
	if (WasInputKeyJustPressed(EKeys::Q))
	{
//		S1Player->OnItemEquipped(S1ItemTags::Item_Weapon_SWD01);
		S1Player->OnItemEquipped(S1ItemTags::Item_Weapon_DSWD01);
	}
	if (WasInputKeyJustPressed(EKeys::E))
	{
		S1Player->OnItemEquipped(FGameplayTag());
	}

	const US1LockOnComponent* LockOnComp = S1Player->GetLockOnComponent();
	if (false == (LockOnComp && LockOnComp->IsLockedOn())) { return; }

	const FVector ToTarget = (LockOnComp->GetLockOnTargetLocation()
		- S1Player->GetActorLocation()).GetSafeNormal2D();
	if (ToTarget.IsNearlyZero()) { return; }

	SetControlRotation(FRotator(GetControlRotation().Pitch, ToTarget.Rotation().Yaw, 0.f));
}

void AS1PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 서버(리슨 호스트 로컬 플레이어 포함) — 원격 클라는 여기 도달 안 함
	InitPawnInput(InPawn);
}

void AS1PlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	LOG(TEXT("PlayerController: AcknowledgePossession — Pawn [%s]"), *GetNameSafe(P));

	// 원격 클라(소유) — 입력 바인딩은 InputComponent가 있는 클라에서 해야 동작
	InitPawnInput(P);

	// SpringArm의 bUsePawnControlRotation은 ControlRotation을 따라가지 Pawn의 실제 회전을 안 봄 —
	// 스폰 Transform의 회전을 반영하려면 Possess 직후 ControlRotation을 Pawn 회전에 맞춰줘야 카메라도 같이 돎
	if (::IsValid(P))
	{
		SetControlRotation(P->GetActorRotation());
	}

	// 부스 확정 후 실제 게임플레이 Pawn으로 처음 Possess된 시점 — 로컬 프리뷰 정리 + UI 전환
	if (::IsValid(CharacterSelectComponent))
	{
		CharacterSelectComponent->HandleGameplayPawnPossessed();
	}
}

void AS1PlayerController::InitPawnInput(APawn* InPawn)
{
	S1Player = Cast<AS1Player>(InPawn);
	if (IsValid(S1Player))
	{
		SetupAbilityInputBindings(S1Player->GetAbilityInputBindings());
	}
}

void AS1PlayerController::SetupAbilityInputBindings(const TArray<FS1AbilityInputBinding>& Bindings)
{
	// 리슨 서버 로컬 플레이어는 OnPossess+AcknowledgePossession 둘 다 여기 도달 — 두 번째 호출 무시
	if (bAbilityInputBound)
	{
		return;
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (false == IsValid(EIC))
	{
		return;
	}

	const US1InputData* InputData = US1AssetManager::GetAssetByTag<US1InputData>(S1AssetTags::Asset_InputData);
	if (false == IsValid(InputData))
	{
		return;
	}

	bAbilityInputBound = true;

	for (const FS1AbilityInputBinding& Binding : Bindings)
	{
		if (const UInputAction* Action = InputData->FindInputActionByTag(Binding.InputTag))
		{
			EIC->BindAction(Action, ETriggerEvent::Started, this, &ThisClass::OnAbilityInput, Binding.AbilityTag);
			EIC->BindAction(Action, ETriggerEvent::Completed, this, &ThisClass::OnAbilityInputReleased, Binding.AbilityTag);
		}
	}
}

void AS1PlayerController::OnInventory(const FInputActionValue& Value)
{
	if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
	{
		if (US1RootWidget* RootWidget = UIManager->GetRootWidget())
		{
			RootWidget->ShowMenu(S1UITags::UI_Menu_Inventory);
			SetCursorMode(RootWidget->IsInventoryMenuOpen());
		}
	}
}

void AS1PlayerController::OnFreeCursorPressed(const FInputActionValue& Value)
{
	// 인벤토리가 열려 있으면 그 상태가 우선 — Alt 입력 무시.
	if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
	{
		if (US1RootWidget* RootWidget = UIManager->GetRootWidget())
		{
			if (RootWidget->IsInventoryMenuOpen())
			{
				return;
			}
		}
	}

	SetCursorMode(true);
}

void AS1PlayerController::OnFreeCursorReleased(const FInputActionValue& Value)
{
	// 인벤토리가 열려 있으면 Alt를 떼도 커서를 닫지 않는다.
	if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
	{
		if (US1RootWidget* RootWidget = UIManager->GetRootWidget())
		{
			if (RootWidget->IsInventoryMenuOpen())
			{
				return;
			}
		}
	}

	SetCursorMode(false);
}

void AS1PlayerController::SetCursorMode(bool bEnable)
{
	bCursorMode = bEnable;

	US1UIManager* UIManager = SUBSYSTEM(US1UIManager);
	US1RootWidget* RootWidget = UIManager ? UIManager->GetRootWidget() : nullptr;

	if (bEnable)
	{
		// 커서가 화면 중앙에서 나타나도록 마우스를 중앙으로 재배치.
		if (UGameViewportClient* ViewportClient = GetWorld() ? GetWorld()->GetGameViewport() : nullptr)
		{
			FVector2D ViewportSize;
			ViewportClient->GetViewportSize(ViewportSize);
			SetMouseLocation(static_cast<int32>(ViewportSize.X * 0.5f), static_cast<int32>(ViewportSize.Y * 0.5f));
		}

		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);

		if (RootWidget)
		{
			RootWidget->SetCursorVisible(true);
		}
	}
	else
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);

		if (RootWidget)
		{
			RootWidget->SetCursorVisible(false);

			if (US1Inventory_ItemInfo* ItemInfoWidget = RootWidget->GetItemInfoWidget())
			{
				ItemInfoWidget->HideInfo();
			}
		}
	}
}

bool AS1PlayerController::IsPlayerAttackAbility(const FGameplayTag& AbilityTag)
{
	return AbilityTag.IsValid() && AbilityTag.MatchesTag(S1AbilityTags::Ability_Player_Attack);
}

void AS1PlayerController::OnAbilityInput(const FInputActionValue& Value, FGameplayTag AbilityTag)
{
	if (false == IsValid(S1Player))
	{
		return;
	}

	if (US1UIManager* UIManager = SUBSYSTEM(US1UIManager))
	{
		if (US1RootWidget* RootWidget = UIManager->GetRootWidget())
		{
			if (RootWidget->IsInventoryMenuOpen() && IsPlayerAttackAbility(AbilityTag))
			{
				return;
			}
		}
	}

	S1Player->ActivateAbility(AbilityTag);
}

void AS1PlayerController::OnAbilityInputReleased(const FInputActionValue& Value, FGameplayTag AbilityTag)
{
	// 릴리즈는 UI 상태와 무관하게 항상 전달 — 홀드 중 메뉴가 열려도 GA가 키 업을 놓치지 않도록
	if (false == IsValid(S1Player))
	{
		return;
	}

	S1Player->ReleaseAbility(AbilityTag);
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

	// 합산 방향 저장 — GA_Action::OnMoveBeginReceived에서 S키(반대 방향) 체크용
	// Triggered마다 갱신, Completed(키 릴리즈)에서 ZeroVector로 리셋
	S1Player->SetLastInputDirection((ForwardDir * MoveVector.Y + RightDir * MoveVector.X).GetSafeNormal());

	S1Player->AddMovementInput(ForwardDir, MoveVector.Y);
	S1Player->AddMovementInput(RightDir,   MoveVector.X);
}

void AS1PlayerController::OnMoveReleased(const FInputActionValue& Value)
{
	if (IsValid(S1Player))
	{
		S1Player->SetLastInputDirection(FVector::ZeroVector);
	}
}

void AS1PlayerController::OnTurn(const FInputActionValue& Value)
{
	// 커서 모드(인벤토리/Alt)에서는 마우스로 카메라를 회전시키지 않는다.
	if (bCursorMode)
	{
		return;
	}

	const FVector2D LookVector = Value.Get<FVector2D>();
	AddYawInput(LookVector.X);
	AddPitchInput(LookVector.Y);
}

void AS1PlayerController::OnSprint(const FInputActionValue& Value)
{
	if (false == IsValid(S1Player))
	{
		return;
	}

	S1Player->SetSprinting(Value.Get<bool>());
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

void AS1PlayerController::OnInteract(const FInputActionValue& Value)
{
	if (false == ::IsValid(S1Player)) { return; }

	S1Player->TryInteract();
}

void AS1PlayerController::OnChangeLockOnSide(const FInputActionValue& Value)
{
	if (false == ::IsValid(S1Player)) { return; }

	if (US1LockOnComponent* Comp = S1Player->GetLockOnComponent())
	{
		Comp->ToggleCameraSide();
	}
}
