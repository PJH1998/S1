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

	// 스프링암 우회 구간의 콜리전 대체 — 피벗→목표 카메라 위치를 스윕해 막히면 충돌 지점으로 당김
	FVector ApplyCameraCollision(const FVector& Pivot, const FVector& DesiredPos, AActor* IgnoreActor) const;

private:
	// 플레이어 기준 카메라 뒤 거리
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Camera")
	float BackDistance = 650.f;

	// 락온 카메라 콜리전 프로브 반경 (스프링암 ProbeSize 상당)
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Camera")
	float ProbeSize = 12.f;

	// 콜리전 프로브 채널 — 기본 Camera (Player/Monster 캡슐은 이 채널을 Ignore)
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Camera")
	TEnumAsByte<ECollisionChannel> ProbeChannel = ECC_Camera;

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
