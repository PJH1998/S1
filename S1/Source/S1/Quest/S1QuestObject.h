// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "S1QuestObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FS1QuestClearedSignature);

/**
 * 태그로 US1QuestManager에 자기 등록하는 전역(스테이지 단위) 퀘스트.
 * 진행 카운트(CurrentCount)가 RequiredCount에 도달하면 OnQuestCleared를 1회 브로드캐스트한다.
 * 클리어 후 처리(기믹 실행, 보상 등)는 이 클래스의 BP 서브클래스가 OnQuestCleared를 직접 바인딩해서 구성한다.
 */
UCLASS()
class S1_API AS1QuestObject : public AActor
{
	GENERATED_BODY()

public:
	AS1QuestObject();

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FS1QuestClearedSignature OnQuestCleared;

	bool IsCleared() const { return CurrentCount >= RequiredCount; }

	// 서버 전용: 진행도를 올린다. 이미 클리어된 퀘스트는 무시(멱등).
	void AddProgress(int32 Amount);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentCount(int32 OldCurrentCount);

protected:
	// US1QuestManager 등록/질의 식별 태그. 레벨에 개별 배치하지 않고 BP 클래스 기본값으로 설정.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FGameplayTag QuestTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 RequiredCount = 1;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentCount, BlueprintReadOnly, Category = "Quest")
	int32 CurrentCount = 0;
};
