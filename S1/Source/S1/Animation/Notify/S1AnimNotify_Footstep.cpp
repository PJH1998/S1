// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_Footstep.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Data/S1DataTableData.h"
#include "S1DataTableTypes.h"
#include "System/S1AssetManager.h"
#include "Tags/S1GameplayTags.h"

void US1AnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (false == ::IsValid(MeshComp))
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	UWorld* World = MeshComp->GetWorld();
	if (nullptr == Owner || nullptr == World || NM_DedicatedServer == World->GetNetMode())
	{
		return;
	}

	const FVector SocketLocation = MeshComp->GetSocketLocation(FootSocketName);
	const FVector TraceStart = SocketLocation + FVector(0.f, 0.f, TraceUpOffset);
	const FVector TraceEnd   = SocketLocation - FVector(0.f, 0.f, TraceDownDistance);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FootstepTrace), false, Owner);
	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult Hit;
	if (false == World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannel, QueryParams))
	{
		return;
	}

	const EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

	US1DataTableData* DTData = US1AssetManager::GetAssetByTag<US1DataTableData>(S1AssetTags::Asset_DataTable);
	UDataTable* FootstepTable = ::IsValid(DTData) ? DTData->GetDataTable(S1DataTableTags::DataTable_FootstepSound) : nullptr;
	if (nullptr == FootstepTable)
	{
		return;
	}

	TArray<FS1FootstepSoundRow*> Rows;
	FootstepTable->GetAllRows<FS1FootstepSoundRow>(TEXT("Footstep"), Rows);

	USoundBase* FootstepSound = nullptr;
	for (const FS1FootstepSoundRow* Row : Rows)
	{
		if (nullptr != Row && Row->SurfaceType == SurfaceType)
		{
			FootstepSound = Row->FootstepSound;
			break;
		}
	}

	if (nullptr == FootstepSound)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(Owner, FootstepSound, Hit.ImpactPoint);
}
