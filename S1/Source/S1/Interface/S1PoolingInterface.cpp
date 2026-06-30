// Fill out your copyright notice in the Description page of Project Settings.

#include "Interface/S1PoolingInterface.h"
#include "System/S1PoolingManager.h"
#include "GameFramework/Actor.h"
#include "S1LogChannels.h"

void IS1PoolingInterface::OnSpawnFromPool(FGameplayTag InPoolTag, FVector Location, FRotator Rotation)
{
	PoolTag = InPoolTag;

	if (AActor* Self = Cast<AActor>(_getUObject()))
	{
		// 스폰=활성화 → 어웨이크로 복귀(내부적으로 flush 포함). FlushNetDormancy만으론 DORM_DormantAll이 유지되어
		// 1회 복제 후 다시 도먼트 → 연속 이동(위치/속도/RepRootMotion)이 클라에 복제되지 않음.
		Self->SetNetDormancy(ENetDormancy::DORM_Awake);
		Self->SetActorLocationAndRotation(Location, Rotation);
		Self->SetActorHiddenInGame(false);
		if (USceneComponent* Root = Self->GetRootComponent())
		{
			Root->SetVisibility(true, true);
		}
		Self->SetActorEnableCollision(true);
		Self->SetActorTickEnabled(true);
		Self->ForceNetUpdate();
	}
}

void IS1PoolingInterface::OnReturnToPool()
{
	if (AActor* Self = Cast<AActor>(_getUObject()))
	{
		Self->SetActorHiddenInGame(true);
		if (USceneComponent* Root = Self->GetRootComponent())
		{
			Root->SetVisibility(false, true);
		}
		Self->SetActorEnableCollision(false);
		Self->SetActorTickEnabled(false);
		Self->SetNetDormancy(ENetDormancy::DORM_DormantAll);
	}
}

void IS1PoolingInterface::ReturnSelf()
{
	AActor* Self = Cast<AActor>(_getUObject());
	if (false == IsValid(Self))
	{
		return;
	}

	US1PoolingManager* PoolingManager = Self->GetWorld()->GetSubsystem<US1PoolingManager>();
	if (false == IsValid(PoolingManager))
	{
		return;
	}

	// 로컬(비리플리케이트) 풀은 클라에서도 반납 허용. 비-로컬은 권위에서만.
	if (false == Self->HasAuthority() && false == PoolingManager->IsLocalPoolTag(PoolTag))
	{
		return;
	}

	PoolingManager->ReturnToPool(Self, PoolTag);
}
