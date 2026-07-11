// Fill out your copyright notice in the Description page of Project Settings.

#include "Quest/S1QuestObject.h"

#include "Net/UnrealNetwork.h"
#include "System/S1QuestManager.h"

AS1QuestObject::AS1QuestObject()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void AS1QuestObject::BeginPlay()
{
	Super::BeginPlay();

	if (US1QuestManager* QuestManager = GetWorld()->GetSubsystem<US1QuestManager>())
	{
		QuestManager->RegisterQuest(QuestTag, this);
	}
}

void AS1QuestObject::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (US1QuestManager* QuestManager = GetWorld()->GetSubsystem<US1QuestManager>())
	{
		QuestManager->UnregisterQuest(QuestTag, this);
	}

	Super::EndPlay(EndPlayReason);
}

void AS1QuestObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS1QuestObject, CurrentCount);
}

void AS1QuestObject::AddProgress(int32 Amount)
{
	if (false == HasAuthority() || IsCleared())
	{
		return;
	}

	const int32 OldCurrentCount = CurrentCount;
	CurrentCount = FMath::Min(CurrentCount + Amount, RequiredCount);
	OnRep_CurrentCount(OldCurrentCount);   // 서버에서도 클리어 판정 적용
	ForceNetUpdate();
}

void AS1QuestObject::OnRep_CurrentCount(int32 OldCurrentCount)
{
	// 방금 막 클리어 문턱을 넘은 순간에만 발화(서버·클라 공통 진입점이라 멱등해야 함).
	if (OldCurrentCount < RequiredCount && CurrentCount >= RequiredCount)
	{
		OnQuestCleared.Broadcast();
	}
}
