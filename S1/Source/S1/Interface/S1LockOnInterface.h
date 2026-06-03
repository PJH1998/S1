#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "S1Enums.h"
#include "S1LockOnInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class US1LockOnInterface : public UInterface
{
	GENERATED_BODY()
};

class S1_API IS1LockOnInterface
{
	GENERATED_BODY()
public:
	// 락온 우선순위 1차: 적 종류 반환
	UFUNCTION(BlueprintNativeEvent, Category = "LockOn")
	ES1EnemyTier GetLockOnTier();

	// 카메라가 바라볼 월드 위치 (보통 가슴/중심 위치)
	UFUNCTION(BlueprintNativeEvent, Category = "LockOn")
	FVector GetLockOnFocusLocation();
};
