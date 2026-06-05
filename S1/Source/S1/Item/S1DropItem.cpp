// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/S1DropItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "S1Define.h"

AS1DropItem::AS1DropItem()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bOnlyRelevantToOwner = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneRoot);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(SceneRoot);
	PickupSphere->SetSphereRadius(PickupRadius);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupSphere->SetCollisionObjectType(ECC_WorldDynamic);
	PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(S1CollisionChannel::CC_Player, ECR_Overlap);
}

void AS1DropItem::BeginPlay()
{
	Super::BeginPlay();

	PickupSphere->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnPickupSphereBeginOverlap);
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnPickupSphereBeginOverlap);
}

void AS1DropItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDropPresentation(DeltaTime);
}

void AS1DropItem::OnSpawnFromPool(FGameplayTag InPoolTag, FVector Location, FRotator Rotation)
{
	IS1PoolingInterface::OnSpawnFromPool(InPoolTag, Location, Rotation);
}

void AS1DropItem::OnReturnToPool()
{
	bIsPresenting = false;
	PresentationElapsedTime = 0.f;
	DropType = ES1DropItemType::Item;
	ItemTag = FGameplayTag();
	RarityTag = FGameplayTag();
	Amount = 0;
	OwnerController.Reset();
	SetOwner(nullptr);

	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(nullptr);
		MeshComponent->SetMaterial(0, nullptr);
		MeshComponent->SetRelativeLocation(FVector::ZeroVector);
	}
	DynamicMaterial = nullptr;

	if (PickupSphere)
	{
		PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	IS1PoolingInterface::OnReturnToPool();
}

void AS1DropItem::InitializeDrop(ES1DropItemType InDropType, int32 InAmount, FGameplayTag InItemTag, FGameplayTag InRarityTag, const FS1DropItemResourceEntry& Resource, AController* InOwnerController)
{
	DropType = InDropType;
	ItemTag = InItemTag;
	RarityTag = InRarityTag;
	Amount = FMath::Max(InAmount, 0);
	OwnerController = InOwnerController;
	SetOwner(InOwnerController);

	switch (DropType)
	{
	case ES1DropItemType::Gold:
		BindGoldResource(Resource);
		break;
	case ES1DropItemType::Exp:
		BindExpResource(Resource);
		break;
	case ES1DropItemType::Item:
	default:
		BindItemResource(Resource);
		break;
	}

	if (PickupSphere)
	{
		PickupSphere->SetSphereRadius(PickupRadius);
		PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	if (Resource.DropEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Resource.DropEffect, GetActorLocation());
	}

	StartDropPresentation();
}

void AS1DropItem::BindGoldResource(const FS1DropItemResourceEntry& Resource)
{
	BindBaseResource(Resource);
}

void AS1DropItem::BindExpResource(const FS1DropItemResourceEntry& Resource)
{
	BindBaseResource(Resource);
}

void AS1DropItem::BindItemResource(const FS1DropItemResourceEntry& Resource)
{
	BindBaseResource(Resource);
}

void AS1DropItem::BindBaseResource(const FS1DropItemResourceEntry& Resource)
{
	if (MeshComponent == nullptr)
	{
		return;
	}

	MeshComponent->SetStaticMesh(Resource.Mesh);
	MeshComponent->SetRelativeLocation(FVector(0.f, 0.f, Resource.MeshZOffset));
	DynamicMaterial = Resource.Material ? MeshComponent->CreateDynamicMaterialInstance(0, Resource.Material) : nullptr;

	if (DynamicMaterial && Resource.Texture)
	{
		DynamicMaterial->SetTextureParameterValue(TEXT("BaseTexture"), Resource.Texture);
	}
}

void AS1DropItem::StartDropPresentation()
{
	PresentationElapsedTime = 0.f;
	PresentationStartLocation = GetActorLocation();
	PresentationTargetLocation = CalculateDropTargetLocation();
	bIsPresenting = PresentationDuration > 0.f;

	if (bIsPresenting == false)
	{
		SetActorLocation(PresentationTargetLocation);
	}
}

void AS1DropItem::UpdateDropPresentation(float DeltaTime)
{
	if (bIsPresenting == false)
	{
		return;
	}

	PresentationElapsedTime += DeltaTime;
	const float Alpha = FMath::Clamp(PresentationElapsedTime / PresentationDuration, 0.f, 1.f);
	FVector NewLocation = FMath::Lerp(PresentationStartLocation, PresentationTargetLocation, Alpha);
	NewLocation.Z += FMath::Sin(Alpha * PI) * PresentationHeight;
	SetActorLocation(NewLocation);

	if (Alpha >= 1.f)
	{
		bIsPresenting = false;
		SetActorLocation(PresentationTargetLocation);
	}
}

FVector AS1DropItem::CalculateDropTargetLocation() const
{
	const FVector StartLocation = GetActorLocation();
	const float Angle = FMath::FRandRange(0.f, 2.f * PI);
	const float Distance = FMath::FRandRange(PresentationDistance * 0.45f, PresentationDistance);
	FVector TargetLocation = StartLocation + FVector(FMath::Cos(Angle) * Distance, FMath::Sin(Angle) * Distance, 0.f);

	if (UWorld* World = GetWorld())
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(DropItemGroundTrace), false, this);
		const FVector TraceStart = TargetLocation + FVector(0.f, 0.f, 100.f);
		const FVector TraceEnd = TargetLocation - FVector(0.f, 0.f, 500.f);
		if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
		{
			TargetLocation = HitResult.ImpactPoint;
		}
	}

	return TargetLocation;
}

bool AS1DropItem::CanPickup(AActor* OtherActor) const
{
	if (false == IsValid(OtherActor))
	{
		return false;
	}

	const APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn == nullptr)
	{
		return false;
	}

	AController* OtherController = Pawn->GetController();
	return OwnerController.IsValid() == false || OwnerController.Get() == OtherController;
}

void AS1DropItem::Pickup()
{
	// TODO: Apply Amount to inventory, gold, or exp when reward systems are ready.
	if (PoolTag.IsValid())
	{
		ReturnSelf();
		return;
	}

	Destroy();
}

void AS1DropItem::OnPickupSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CanPickup(OtherActor))
	{
		Pickup();
	}
}

