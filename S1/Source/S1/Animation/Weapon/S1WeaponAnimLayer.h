#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "S1WeaponAnimLayer.generated.h"

class US1PlayerAnimInstance;

UCLASS()
class S1_API US1WeaponAnimLayer : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;


	UFUNCTION(BlueprintPure)
	US1PlayerAnimInstance* GetPlayerAnimInstance() const;

protected:
	// US1PlayerAnimInstance에서 매 프레임 미러링
	UPROPERTY(BlueprintReadOnly)
	float GroundSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool bMove = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly)
	bool bSprint = false;

	UPROPERTY(BlueprintReadOnly)
	bool bDirectToFall = false;

	UPROPERTY(BlueprintReadOnly)
	bool bWait = false;

	// State Machine이 Layer로 이동했으므로 소유권 이동
	UPROPERTY(BlueprintReadOnly)
	bool bRunLoop = false;
};
