// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Gimmick/S1GimmickObject.h"
#include "S1PuzzleButton_Gimmick.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AS1TogglePillar_Gimmick;
class AS1PuzzleButton_Gimmick;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FS1ButtonToggledSignature, AS1PuzzleButton_Gimmick*, Button);

/**
 * 플레이어가 Interact 키(US1InteractComponent 경유)로 조작하는 버튼.
 * 연결된 기둥(ConnectedPillars)의 최고/최저 상태를 On/Off처럼 뒤집는다.
 * 자신의 bIsActivated는 퍼즐 판정과 무관한 레버 눌림 비주얼 용도로만 재사용.
 */
UCLASS()
class S1_API AS1PuzzleButton_Gimmick : public AS1GimmickObject
{
	GENERATED_BODY()

public:
	AS1PuzzleButton_Gimmick();

	// 서버 전용: 연결된 기둥들을 전부 토글하고 OnButtonToggled를 브로드캐스트한다.
	void PressButton();

	// 서버 전용: 코디네이터가 스폰 직후 이 버튼이 담당할 기둥들을 주입한다.
	void SetConnectedPillars(const TArray<AS1TogglePillar_Gimmick*>& InPillars);

	// 코디네이터 전용: 퍼즐이 완료되면 호출해 잠근다 — 이후 PressButton()은 아무 일도 하지 않는다.
	void SetPuzzleLocked(bool bLocked);

	const TArray<TObjectPtr<AS1TogglePillar_Gimmick>>& GetConnectedPillars() const { return ConnectedPillars; }

	UPROPERTY(BlueprintAssignable, Category = "Gimmick|PuzzleButton")
	FS1ButtonToggledSignature OnButtonToggled;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ButtonMesh;

	// US1InteractComponent의 탐지 스피어가 겹칠 수 있도록 별도로 둔 오버랩 전용 볼륨.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> InteractionCollision;

	// 이 버튼을 누르면 On/Off처럼 상태가 뒤집히는 기둥들 — 코디네이터가 SetConnectedPillars로
	// 스폰 직후 주입한다(레벨 디자이너가 손으로 배선하지 않음).
	UPROPERTY(Transient)
	TArray<TObjectPtr<AS1TogglePillar_Gimmick>> ConnectedPillars;

	// 퍼즐 완료 후 true — PressButton()이 더 이상 기둥을 움직이지 않도록 막는다.
	bool bPuzzleLocked = false;
};