// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "S1SequenceManager.generated.h"

class ULevelSequencePlayer;

// SequenceTag로 ULevelSequencePlayer를 미리 생성해 캐싱해두고, 필요할 때 태그로 찾아 재생하는 서브시스템.
// US1GimmickManager와 동일하게 서버/각 클라에 독립적으로 존재 - RPC 없이 호출된 머신에서 로컬로만 동작한다.
UCLASS()
class S1_API US1SequenceManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// SequenceTag로 US1SequenceResource(Asset_SequenceResource)에서 시퀀스를 찾아 미리 생성(bAutoPlay=false)해 캐싱한다.
	// 이미 캐싱돼 있으면 기존 걸 반환.
	UFUNCTION(BlueprintCallable, Category = "Sequence")
	ULevelSequencePlayer* PrepareSequence(FGameplayTag SequenceTag);

	// 캐싱된 플레이어를 태그로 찾아 재생한다. Prepare가 선행되지 않았으면 nullptr을 반환한다.
	UFUNCTION(BlueprintCallable, Category = "Sequence")
	ULevelSequencePlayer* PlaySequenceByTag(FGameplayTag SequenceTag);

	// 캐싱된 플레이어를 태그로 찾아 즉시 정지한다(엔진 Stop()은 커서를 시퀀스 끝(또는 역재생이면 시작)으로 이동).
	// 시퀀스 길이를 재생 종료 타이밍에 맞춰 제작할 필요 없이, 호출부가 원하는 시점에 명시적으로 끊는 용도.
	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void StopSequenceByTag(FGameplayTag SequenceTag);

private:
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<ULevelSequencePlayer>> CachedPlayers;
};