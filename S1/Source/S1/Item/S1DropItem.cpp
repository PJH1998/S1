// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/S1DropItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "S1Define.h"
#include "System/S1AssetManager.h"
#include "System/S1ItemManager.h"
#include "Tags/S1GameplayTags.h"

namespace
{
	const FName BaseTextureParameterName = TEXT("BaseTexture");
	const FName TintColorParameterName = TEXT("TintColor");
	const FName EmissiveColorParameterName = TEXT("EmissiveColor");
	const FName EmissiveIntensityParameterName = TEXT("EmissiveIntensity");
	const FName MetallicParameterName = TEXT("Metallic");
	const FName RoughnessParameterName = TEXT("Roughness");
	const FName NoiseTextureParameterName = TEXT("NoiseTexture");
	const FName NoiseScaleParameterName = TEXT("NoiseScale");
	const FName NoisePhaseParameterName = TEXT("NoisePhase");
	const FName FlashIntensityParameterName = TEXT("FlashIntensity");
	const FName FlashSpeedParameterName = TEXT("FlashSpeed");
	const FName FlashWidthParameterName = TEXT("FlashWidth");
	const FName FlashSoftnessParameterName = TEXT("FlashSoftness");
	const FName FlashContrastParameterName = TEXT("FlashContrast");
}

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

void AS1DropItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS1DropItem, DropType);
	DOREPLIFETIME(AS1DropItem, ItemTag);
	DOREPLIFETIME(AS1DropItem, RarityTag);
	DOREPLIFETIME(AS1DropItem, OwnerController);
	DOREPLIFETIME(AS1DropItem, Amount);
	DOREPLIFETIME(AS1DropItem, ReplicatedSpawnLocation);
}

void AS1DropItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDropPresentation(DeltaTime);
	UpdateFloatingMotion(DeltaTime);
}

void AS1DropItem::OnSpawnFromPool(FGameplayTag InPoolTag, FVector Location, FRotator Rotation)
{
	IS1PoolingInterface::OnSpawnFromPool(InPoolTag, Location, Rotation);
}

void AS1DropItem::OnReturnToPool()
{
	bIsPresenting = false;
	PresentationElapsedTime = 0.f;
	FloatingElapsedTime = 0.f;
	DropType = ES1DropItemType::Item;
	ItemTag = FGameplayTag();
	RarityTag = FGameplayTag();
	Amount = 0;
	ReplicatedSpawnLocation = FVector::ZeroVector;
	OwnerController = nullptr;
	SetOwner(nullptr);
	MeshBaseRelativeLocation = FVector::ZeroVector;

	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(nullptr);
		MeshComponent->SetRelativeLocation(FVector::ZeroVector);
	}

	if (PickupSphere)
	{
		PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	IS1PoolingInterface::OnReturnToPool();
	ForceNetUpdate();
}

void AS1DropItem::InitializeDrop(ES1DropItemType InDropType, int32 InAmount, FGameplayTag InItemTag, FGameplayTag InRarityTag, const FS1DropItemResourceEntry& Resource, const FS1DropItemVisualParams& VisualParams, AController* InOwnerController)
{
	DropType = InDropType;
	ItemTag = InItemTag;
	RarityTag = InRarityTag;
	Amount = FMath::Max(InAmount, 0);
	FloatingElapsedTime = 0.f;
	ReplicatedSpawnLocation = GetActorLocation();
	OwnerController = InOwnerController;
	SetOwner(InOwnerController);

	switch (DropType)
	{
	case ES1DropItemType::Gold:
		BindGoldResource(Resource, VisualParams);
		break;
	case ES1DropItemType::Exp:
		BindExpResource(Resource, VisualParams);
		break;
	case ES1DropItemType::Item:
	default:
		BindItemResource(Resource, VisualParams);
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
	ForceNetUpdate();
}

void AS1DropItem::BindGoldResource(const FS1DropItemResourceEntry& Resource, const FS1DropItemVisualParams& VisualParams)
{
	BindBaseResource(Resource, VisualParams);
}

void AS1DropItem::BindExpResource(const FS1DropItemResourceEntry& Resource, const FS1DropItemVisualParams& VisualParams)
{
	BindBaseResource(Resource, VisualParams);
}

void AS1DropItem::BindItemResource(const FS1DropItemResourceEntry& Resource, const FS1DropItemVisualParams& VisualParams)
{
	BindBaseResource(Resource, VisualParams);
}

void AS1DropItem::BindBaseResource(const FS1DropItemResourceEntry& Resource, const FS1DropItemVisualParams& VisualParams)
{
	if (MeshComponent == nullptr)
	{
		return;
	}

	MeshComponent->SetStaticMesh(Resource.Mesh);
	MeshBaseRelativeLocation = FVector(0.f, 0.f, Resource.MeshZOffset);
	MeshComponent->SetRelativeLocation(MeshBaseRelativeLocation);

	if (Resource.Material == nullptr)
	{
		MeshComponent->SetMaterial(0, nullptr);
		DynamicMaterial = nullptr;
		DynamicMaterialSource = nullptr;
		return;
	}

	if (DynamicMaterial == nullptr || DynamicMaterialSource.Get() != Resource.Material.Get())
	{
		DynamicMaterial = MeshComponent->CreateDynamicMaterialInstance(0, Resource.Material.Get());
		DynamicMaterialSource = Resource.Material;
	}
	else
	{
		MeshComponent->SetMaterial(0, DynamicMaterial);
	}

	ApplyVisualParams(Resource, VisualParams);
}

void AS1DropItem::ApplyVisualParams(const FS1DropItemResourceEntry& Resource, const FS1DropItemVisualParams& VisualParams)
{
	if (DynamicMaterial == nullptr)
	{
		return;
	}

	DynamicMaterial->SetTextureParameterValue(BaseTextureParameterName, Resource.Texture.Get());
	DynamicMaterial->SetVectorParameterValue(TintColorParameterName, VisualParams.TintColor);
	DynamicMaterial->SetVectorParameterValue(EmissiveColorParameterName, VisualParams.EmissiveColor);
	DynamicMaterial->SetScalarParameterValue(EmissiveIntensityParameterName, VisualParams.EmissiveIntensity);
	DynamicMaterial->SetScalarParameterValue(MetallicParameterName, VisualParams.Metallic);
	DynamicMaterial->SetScalarParameterValue(RoughnessParameterName, VisualParams.Roughness);
	DynamicMaterial->SetTextureParameterValue(NoiseTextureParameterName, VisualParams.NoiseTexture.Get());
	DynamicMaterial->SetScalarParameterValue(NoiseScaleParameterName, VisualParams.NoiseScale);
	DynamicMaterial->SetScalarParameterValue(NoisePhaseParameterName, FMath::FRandRange(0.f, 1000.f));
	DynamicMaterial->SetScalarParameterValue(FlashIntensityParameterName, VisualParams.FlashIntensity);
	DynamicMaterial->SetScalarParameterValue(FlashSpeedParameterName, VisualParams.FlashSpeed);
	DynamicMaterial->SetScalarParameterValue(FlashWidthParameterName, VisualParams.FlashWidth);
	DynamicMaterial->SetScalarParameterValue(FlashSoftnessParameterName, VisualParams.FlashSoftness);
	DynamicMaterial->SetScalarParameterValue(FlashContrastParameterName, VisualParams.FlashContrast);
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

void AS1DropItem::UpdateFloatingMotion(float DeltaTime)
{
	if (bIsPresenting || MeshComponent == nullptr)
	{
		return;
	}

	if (DropType != ES1DropItemType::Item && DropType != ES1DropItemType::Exp)
	{
		MeshComponent->SetRelativeLocation(MeshBaseRelativeLocation);
		return;
	}

	FloatingElapsedTime += DeltaTime;
	FVector NewRelativeLocation = MeshBaseRelativeLocation;
	NewRelativeLocation.Z += FMath::Sin(FloatingElapsedTime * FloatingSpeed) * FloatingAmplitude;
	MeshComponent->SetRelativeLocation(NewRelativeLocation);
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
	return OwnerController == nullptr || OwnerController == OtherController;
}

void AS1DropItem::Pickup()
{
	if (false == HasAuthority())
	{
		return;
	}

	US1ItemManager* ItemManager = GetWorld() ? GetWorld()->GetSubsystem<US1ItemManager>() : nullptr;
	if (false == IsValid(ItemManager) || ItemManager->ApplyPickup(OwnerController, DropType, ItemTag, RarityTag, Amount) == false)
	{
		return;
	}

	if (PoolTag.IsValid())
	{
		ReturnSelf();
		return;
	}

	Destroy();
}

void AS1DropItem::ApplyReplicatedDropState()
{
	if (Amount <= 0)
	{
		return;
	}

	US1DropItemResource* DropItemResource = US1AssetManager::GetAssetByTag<US1DropItemResource>(S1AssetTags::Asset_DropItemResource);
	if (false == IsValid(DropItemResource))
	{
		return;
	}

	const FS1DropItemResourceEntry* Resource = DropItemResource->FindResource(DropType, ItemTag, RarityTag);
	if (Resource == nullptr)
	{
		return;
	}

	const FS1DropItemVisualParams& VisualParams = DropItemResource->FindVisualParams(DropType, RarityTag);
	switch (DropType)
	{
	case ES1DropItemType::Gold:
		BindGoldResource(*Resource, VisualParams);
		break;
	case ES1DropItemType::Exp:
		BindExpResource(*Resource, VisualParams);
		break;
	case ES1DropItemType::Item:
	default:
		BindItemResource(*Resource, VisualParams);
		break;
	}

	if (PickupSphere)
	{
		PickupSphere->SetSphereRadius(PickupRadius);
		PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	// 클라이언트는 서버 기준 스폰 위치로 Actor를 맞춘 뒤 프레젠테이션 시작
	if (ReplicatedSpawnLocation != FVector::ZeroVector)
	{
		SetActorLocation(ReplicatedSpawnLocation);
	}
	StartDropPresentation();
}

void AS1DropItem::OnRep_DropState()
{
	ApplyReplicatedDropState();
}

void AS1DropItem::OnPickupSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (false == HasAuthority())
	{
		return;
	}

	if (CanPickup(OtherActor))
	{
		Pickup();
	}
}

