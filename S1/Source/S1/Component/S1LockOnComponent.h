#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "S1Enums.h"
#include "S1LockOnComponent.generated.h"

class USphereComponent;

struct FS1LockOnEntry
{
	TWeakObjectPtr<AActor> Target;
	float                  SnapshotDistance = 0.f;
	float                  SignedAngle      = 0.f; // radians, 음수=왼쪽, 양수=오른쪽

	bool IsValid() const { return Target.IsValid(); }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class S1_API US1LockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	US1LockOnComponent();

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void OnLockOnInput(const FRotator& CameraRotation);
	void ToggleCameraSide();
	void CycleLeft(const FRotator& CameraRotation);
	void CycleRight(const FRotator& CameraRotation);

	bool          IsLockedOn()              const { return CurrentIndex != INDEX_NONE; }
	AActor*       GetCurrentTarget()        const;
	FVector       GetLockOnTargetLocation() const;
	ES1LockOnSide GetCameraSide()           const { return CameraSide; }

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	void BuildAndStartCycle(const FRotator& CameraRotation);
	void InsertIntoActiveCycle(AActor* NewActor);
	void RemoveFromCycle(AActor* Target);
	void ReleaseLockOn();

	void           RefreshCycleOrder(const FRotator& CameraRotation);
	FS1LockOnEntry MakeEntry(AActor* Actor) const;
	float          ComputeSignedAngle(AActor* Actor) const;

private:
	// 뷰포트에서 보이고 에디터에서 채널 설정 가능
	UPROPERTY(VisibleAnywhere, Category = "LockOn")
	TObjectPtr<USphereComponent> LockOnSphere;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float LockOnRadius = 1500.f;

	TSet<TWeakObjectPtr<AActor>> CandidateSet;
	TArray<FS1LockOnEntry>       CycleList;
	int32                        CurrentIndex  = INDEX_NONE;
	ES1LockOnSide                CameraSide    = ES1LockOnSide::Right;

	// BuildAndStartCycle 시점의 카메라 기준 벡터 (InsertIntoActiveCycle에서 재사용)
	FVector CycleForward2D = FVector::ForwardVector;
	FVector CycleRight2D   = FVector::RightVector;
};
