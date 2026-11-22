// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "S1AnimNotifyState_HitLaunch.generated.h"

// 피격 넉백(Strong/ToAir) 구간을 표시하는 데이터 마커 — 아무것도 실행하지 않는다.
// US1GameplayAbility_Hit가 활성화 시점에 몽타주를 스캔해 TriggerTime/Duration을 읽고
// 루트모션 태스크를 만든다(§04 AtkCollision과 동일 패턴).
//
// 과거엔 이 노티파이가 NotifyBegin/End에서 직접 CMC에 소스를 넣고 뺐는데, 데디 서버에서
// NotifyBegin/End가 매 프레임 재발화(thrash)하면서 NotifyEnd가 1프레임 만에 소스를 철거하고
// 넉백 속도까지 0으로 지워버렸다 — 넉백이 적용됐다가 즉시 제자리로 돌아오는 증상으로 측정됐다.
UCLASS()
class S1_API US1AnimNotifyState_HitLaunch : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	// 넉백 루트모션 소스 이름 — GA가 소스를 만들 때 사용
	static const FName RootMotionSourceName;

	// true면 구간이 끝나는 순간의 속도를 그대로 유지(ToAir Start 구간 — 이후 중력받아 포물선 낙하)
	// false면 정지(Strong 등 제자리로 끝나는 지상 넉백)
	bool ShouldMaintainVelocityOnFinish() const { return bMaintainVelocityOnFinish; }

private:
	UPROPERTY(EditAnywhere, Category = "HitLaunch")
	bool bMaintainVelocityOnFinish = false;
};