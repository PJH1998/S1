// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "S1PlayerController.generated.h"

class AS1Player;
struct FS1AbilityInputBinding;
class US1CharacterSelectComponent;

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

	US1CharacterSelectComponent* GetCharacterSelectComponent() const { return CharacterSelectComponent; }

	// State.Dead.CanRespawn 없으면 조용히 무시. 키 입력(OnRespawnInput)과 UI 버튼 둘 다 여기로 수렴.
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void TryRespawn();

	// 커서 표시+Input Mode(GameAndUI)+카메라 회전 정지를 한 번에 처리 — 인벤토리/Alt 프리커서와 동일 메커니즘.
	// 원래 private였으나 US1HUD_Gameplay(리스폰 프롬프트)가 외부에서 호출해야 해서 public으로 전환.
	void SetCursorMode(bool bEnable);

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
	void OnInteract(const FInputActionValue& Value);
	void OnRespawnInput(const FInputActionValue& Value);

	// 서버 권위로 재검증(State.Dead.CanRespawn) 후 EXP 페널티/UltiGauge 초기화 + GA_Death 종료 + 기존 Pawn Destroy
	// + 같은 클래스로 새 Pawn Spawn/Possess(LastSpawnPointTag 위치).
	UFUNCTION(Server, Reliable)
	void ServerRespawn();

private:
	void OnInventory(const FInputActionValue& Value);
	void OnFreeCursorPressed(const FInputActionValue& Value);
	void OnFreeCursorReleased(const FInputActionValue& Value);
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1CharacterSelectComponent> CharacterSelectComponent;

	// 커서 모드(인벤토리 열림 또는 Alt 홀드): 커서 표시 + 마우스 자유 + 카메라 회전 멈춤.
	bool bCursorMode = false;

	// 리슨 서버 로컬 플레이어는 OnPossess(서버)+AcknowledgePossession(소유 클라) 둘 다 이 PC에서 호출됨
	// → SetupAbilityInputBindings 중복 호출 방지 (안 막으면 어빌리티 입력이 매번 두 번 처리되어 콤보가 한 타 건너뜀)
	bool bAbilityInputBound = false;

	// 리스폰 Fade In/Out 길이(초) — US1Customize::OnStartClicked와 동일하게 FadeOut 콜백에서 서버 요청 후 바로 FadeIn.
	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	float RespawnFadeDuration = 1.5f;

	// 리스폰 페널티로 차감할 현재 XP 비율(0~1). 수치 미정 — 밸런스 조정 필요.
	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	float RespawnXPPenaltyPercent = 0.5f;
};
