// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/Decal/S1Decal.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

AS1Decal::AS1Decal()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetupAttachment(SceneRoot);
	DecalComponent->SetVisibility(false);
	DecalComponent->DecalSize = FVector(128.f, 128.f, 128.f);

	SetActorEnableCollision(false);
}

void AS1Decal::BeginPlay()
{
	Super::BeginPlay();

	HideDecal();

	// 객체 생성(=풀 생성, 로딩 중) 시점에 MID를 생성·바인딩하여 셰이더/PSO 컴파일을 일찍 트리거한다.
	// (첫 ShowDecal 때 lazy 생성하면 그 순간 컴파일 폴백으로 화면 전체 플래시가 발생함)
	CreateAndApplyDynamicMaterial();
}

void AS1Decal::OnSpawnFromPool(FGameplayTag InPoolTag, FVector Location, FRotator Rotation)
{
	IS1PoolingInterface::OnSpawnFromPool(InPoolTag, Location, Rotation);

	PoolTag = InPoolTag;

	// 베이스 인터페이스 기본구현(un-hide/collision/tick)은 호출하지 않는다.
	// 데칼은 충돌/네트워크가 없고, 파라미터 적용 전 노출되면 안 되므로 활성화(ShowDecal) 전까진 숨김 유지.
	SetActorLocationAndRotation(Location, Rotation);
}

void AS1Decal::OnReturnToPool()
{
	IS1PoolingInterface::OnReturnToPool();

	HideDecal();
}

void AS1Decal::ShowDecal(const FVector& InLocation, const FRotator& InRotation, const FVector& InDecalSize)
{
	SetActorLocationAndRotation(InLocation, InRotation);

	if (DecalComponent)
	{
		DecalComponent->DecalSize = InDecalSize;
	}

	// 머티리얼(셰이더맵)이 준비되기 전엔 노출하지 않는다(콜드 캐시 첫 렌더 시 기본 머티리얼 폴백=화면 깜빡임 방지).
	// 실제 노출은 UpdateDecal -> TryReveal에서 IsComplete 확인 후 수행.
	bPendingReveal = true;
	bIsActive = true;
}

void AS1Decal::HideDecal()
{
	if (DecalComponent)
	{
		DecalComponent->SetVisibility(false);
	}

	SetActorHiddenInGame(true);
	bIsActive = false;
	bPendingReveal = false;
}

bool AS1Decal::UpdateDecal(float DeltaTime)
{
	if (false == IsActive())
	{
		return true;
	}

	TryReveal();
	return false;
}

bool AS1Decal::TryReveal()
{
	if (false == bPendingReveal)
	{
		return true;
	}

	if (DynamicMaterial == nullptr || false == DynamicMaterial->IsComplete())
	{
		return false;
	}

	if (DecalComponent)
	{
		DecalComponent->SetVisibility(true);
	}

	SetActorHiddenInGame(false);
	bPendingReveal = false;
	return true;
}

UMaterialInstanceDynamic* AS1Decal::CreateAndApplyDynamicMaterial()
{
	if (DynamicMaterial == nullptr)
	{
		if (DecalComponent && DecalComponent->GetDecalMaterial())
		{
			DynamicMaterial = UMaterialInstanceDynamic::Create(DecalComponent->GetDecalMaterial(), this);
		}
	}

	if (DecalComponent && DynamicMaterial)
	{
		DecalComponent->SetDecalMaterial(DynamicMaterial);
	}

	return DynamicMaterial;
}

