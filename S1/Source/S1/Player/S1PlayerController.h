// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "S1PlayerController.generated.h"

class AS1Player;
struct FS1AbilityInputBinding;

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
	void OnMoveReleased(const FInputActionValue& Value);
	void OnTurn(const FInputActionValue& Value);
	void OnSprint(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnLockOn(const FInputActionValue& Value);
	void OnChangeLockOnSide(const FInputActionValue& Value);
	void OnCycleLeft(const FInputActionValue& Value);
	void OnCycleRight(const FInputActionValue& Value);

private:
	void OnInventory(const FInputActionValue& Value);
	void OnFreeCursorPressed(const FInputActionValue& Value);
	void OnFreeCursorReleased(const FInputActionValue& Value);
	void SetCursorMode(bool bEnable);
	static bool IsPlayerAttackAbility(const FGameplayTag& AbilityTag);

	void OnAbilityInput(const FInputActionValue& Value, FGameplayTag AbilityTag);
	void OnAbilityInputReleased(const FInputActionValue& Value, FGameplayTag AbilityTag);
	void SetupAbilityInputBindings(const TArray<FS1AbilityInputBinding>& Bindings);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	// 클라 측 빙의 콜백 — 원격 클라는 OnPossess(서버 전용)가 호출 안 됨
	virtual void AcknowledgePossession(APawn* P) override;

private:
	// 서버/클라 공통 — S1Player 캐싱 + 어빌리티 입력 바인딩
	void InitPawnInput(APawn* InPawn);

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AS1Player> S1Player;

	// 커서 모드(인벤토리 열림 또는 Alt 홀드): 커서 표시 + 마우스 자유 + 카메라 회전 멈춤.
	bool bCursorMode = false;

	// 리슨 서버 로컬 플레이어는 OnPossess(서버)+AcknowledgePossession(소유 클라) 둘 다 이 PC에서 호출됨
	// → SetupAbilityInputBindings 중복 호출 방지 (안 막으면 어빌리티 입력이 매번 두 번 처리되어 콤보가 한 타 건너뜀)
	bool bAbilityInputBound = false;
};
