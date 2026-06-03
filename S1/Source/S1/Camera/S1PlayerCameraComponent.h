#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "S1Enums.h"
#include "S1PlayerCameraComponent.generated.h"

class US1LockOnComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class S1_API US1PlayerCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	US1PlayerCameraComponent();

	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

private:
	US1LockOnComponent* GetLockOnComponent() const;

private:
	// 플레이어 기준 카메라 뒤 거리
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Camera")
	float BackDistance = 400.f;

	// 좌/우 옆 거리
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Camera")
	float SideOffset = 150.f;

	// 카메라 높이 오프셋
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Camera")
	float HeightOffset = 100.f;

	// 시선 혼합 (0=플레이어, 1=타겟)
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Camera", meta = (ClampMin = 0.f, ClampMax = 1.f))
	float LookAtBlend = 0.3f;

	// 위치 보간 속도
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Camera")
	float PositionInterpSpeed = 8.f;

	// 회전 보간 속도
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Camera")
	float RotationInterpSpeed = 10.f;

	// 프레임 간 보간 상태 유지
	FVector  CurrentLockOnPos = FVector::ZeroVector;
	FRotator CurrentLockOnRot = FRotator::ZeroRotator;
	bool     bWasLockedOn     = false;
	bool     bIsBlendingOut   = false;
};
