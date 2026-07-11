// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "S1GimmickManager.generated.h"

class AS1GimmickObject;

// 레벨 내 활성 기믹 목록 — AS1GimmickObject::BeginPlay/EndPlay에서 태그별로 등록/해제.
// 보스 어빌리티는 보스 클래스에 기둥을 직접 바인딩하지 않고 이 매니저에 태그로 질의한다.
UCLASS()
class S1_API US1GimmickManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterGimmick(FGameplayTag Tag, AS1GimmickObject* Gimmick);
	void UnregisterGimmick(FGameplayTag Tag, AS1GimmickObject* Gimmick);

	// 해당 태그 그룹에서 사용 가능한(안 부서진) 기믹 중 랜덤 1개. Exclude는 제외(현재 올라탄 기둥 회피).
	AS1GimmickObject* GetRandomUsableGimmick(FGameplayTag Tag, AS1GimmickObject* Exclude = nullptr) const;
	// From에서 가장 가까운 사용 가능 기믹.
	AS1GimmickObject* GetNearestUsableGimmick(FGameplayTag Tag, const FVector& From) const;

	// 해당 태그 그룹의 모든 기믹을 원상 복구(서버 전용, 보스 리셋 시).
	void ResetGimmicks(FGameplayTag Tag);

	// 해당 태그의 사용 가능한 기믹 하나를 찾아 활성화(서버 전용). 퀘스트 클리어 등 BP에서 직접 호출하는 진입점.
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	void ActivateGimmick(FGameplayTag Tag);

private:
	TMap<FGameplayTag, TArray<TWeakObjectPtr<AS1GimmickObject>>> ActiveGimmicks;
};
