// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "GameplayTagContainer.h"
#include "S1MontageProgression_SequenceBranch.generated.h"

class UAbilityTask_PlayMontageAndWait;

// 한 몽타주의 섹션으로 Start → (분기) End_X 를 진행한다.
//     Start 섹션 재생 → GA가 OnBranchRequested(Key) 호출 → EndSectionNames[Key] 섹션으로 즉시 점프
// 분기 조건(Air/Ground 등)은 GA가 직접 판단해 태그로 전달한다.
//
// 즉시 점프인 이유: 호출부(Assault)가 Start 섹션 안의 MoveEvent 노티파이 종료 시점에 분기를
// 요청하므로, 몽타주를 갈아끼워 Begin을 끊던 기존 동작의 등가물이 점프다.
UCLASS()
class S1_API US1MontageProgression_SequenceBranch : public US1MontageProgression
{
	GENERATED_BODY()

public:
	virtual void OnActivated() override;
	virtual void OnDeactivated() override;
	virtual void OnBranchRequested(FGameplayTag InKey) override;
	virtual bool WillHandleAbilityEnd() const override { return true; }

private:
	void ReleaseMontageTask();

	// 태스크 델리게이트는 무인자 — 어빌리티 종료 시 태스크가 델리게이트를 직접 끊으므로 지연 콜백 가드가 불필요하다
	UFUNCTION() void OnMontageFinished();
	UFUNCTION() void OnMontageAborted();

private:
	// 섹션 구조를 담은 단일 몽타주 — [Start][End_A][End_B] ...
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> SectionMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	FName StartSectionName = TEXT("Start");

	// Key → End 섹션 이름 (BP 에디터에서 행 추가로 분기 확장)
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TMap<FGameplayTag, FName> EndSectionNames;

	// 분기를 못 받아도 몽타주가 끝나지 않도록 Start가 흘러갈 기본 End 섹션.
	// ⚠ None으로 두면 Start 경계에서 몽타주가 종료되는데, 호출부가 Start 섹션 안 MoveEvent의
	//   NotifyEnd에서 분기를 요청하므로 노티파이가 섹션 끝까지 걸친 몽타주에서 경쟁이 생긴다.
	//   큐드 노티파이는 프레임 후반에 처리되어 몽타주 종료가 먼저 이기고, 그러면 태스크의
	//   OnCompleted가 어빌리티를 끝내버려 분기 점프와 MoveEvent가 함께 죽는다
	//   (M_DSWD_Assault 2개에서 실측 — 분기 요청 시 재생중=0, 재생위치=0).
	//   분기 점프가 이 값을 덮어쓰므로 정상 경로에는 영향이 없다
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	FName FallbackEndSectionName = TEXT("End_Ground");

	// 섹션 점프는 하드컷 — 몽타주 분리와 달리 Blend In/Out이 안 붙는다 (0이면 비활성)
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	float BranchInertializationBlendTime = 0.f;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ActiveMontageTask;
};