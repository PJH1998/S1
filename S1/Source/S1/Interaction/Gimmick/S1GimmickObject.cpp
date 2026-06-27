// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/Gimmick/S1GimmickObject.h"

#include "Net/UnrealNetwork.h"
#include "System/S1GimmickManager.h"

AS1GimmickObject::AS1GimmickObject()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void AS1GimmickObject::BeginPlay()
{
	Super::BeginPlay();

	if (US1GimmickManager* GimmickManager = GetWorld()->GetSubsystem<US1GimmickManager>())
	{
		GimmickManager->RegisterGimmick(GimmickTag, this);
	}
}

void AS1GimmickObject::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (US1GimmickManager* GimmickManager = GetWorld()->GetSubsystem<US1GimmickManager>())
	{
		GimmickManager->UnregisterGimmick(GimmickTag, this);
	}

	Super::EndPlay(EndPlayReason);
}

void AS1GimmickObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS1GimmickObject, bIsBroken);
}

void AS1GimmickObject::BreakGimmick()
{
	if (false == HasAuthority() || bIsBroken)
	{
		return;
	}

	bIsBroken = true;
	OnRep_IsBroken();   // 서버에서도 연출 적용
	ForceNetUpdate();
}

void AS1GimmickObject::ResetGimmick()
{
	if (false == HasAuthority() || false == bIsBroken)
	{
		return;
	}

	bIsBroken = false;
	OnRep_IsBroken();
	ForceNetUpdate();
}

void AS1GimmickObject::OnRep_IsBroken()
{
	// 베이스는 상태만 보유. 메쉬 스왑 등 연출은 자식에서 override.
}
