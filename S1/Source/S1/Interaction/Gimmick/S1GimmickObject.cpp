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

	DOREPLIFETIME(AS1GimmickObject, bIsActivated);
}

void AS1GimmickObject::ActivateGimmick()
{
	if (false == HasAuthority() || bIsActivated)
	{
		return;
	}

	bIsActivated = true;
	OnRep_IsActivated();   // 서버에서도 연출 적용
	ForceNetUpdate();
}

void AS1GimmickObject::ResetGimmick()
{
	if (false == HasAuthority() || false == bIsActivated)
	{
		return;
	}

	bIsActivated = false;
	OnRep_IsActivated();
	ForceNetUpdate();
}

void AS1GimmickObject::OnRep_IsActivated()
{
	// 베이스는 상태만 보유. 메쉬 스왑 등 연출은 자식에서 override.

	// 데디서버 제외 — 서버에서 발화하면 BP가 원격 PC에 SetViewTarget 등을 호출하게 되고,
	// 엔진(APlayerCameraManager::AssignViewTarget)이 그 클라에 ClientSetViewTarget RPC를 쏴서
	// 해당 클라의 로컬 카메라 연출을 덮어써버린다.
	if (bIsActivated && GetNetMode() != NM_DedicatedServer)
	{
		OnActivatedCosmetic.Broadcast();
	}
}
