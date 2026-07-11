// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/Gimmick/Sequence/S1GimmickPuzzle_Sequence.h"

#include "Components/BillboardComponent.h"
#include "Data/S1EffectData.h"
#include "Effect/S1Effect.h"
#include "Engine/World.h"
#include "Interaction/Gimmick/S1PressurePlate_Gimmick.h"
#include "System/S1AssetManager.h"
#include "Tags/S1GameplayTags.h"

AS1GimmickPuzzle_Sequence::AS1GimmickPuzzle_Sequence()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	PlateClass = AS1PressurePlate_Gimmick::StaticClass();
}

void AS1GimmickPuzzle_Sequence::ResetGimmick()
{
	Super::ResetGimmick();

	// 완료 여부(bIsActivated)와 무관하게 오답/외부 리셋 요청 시 항상 진행 상태를 초기화한다.
	CurrentStepIndex = 0;

	for (AS1PressurePlate_Gimmick* Plate : SpawnedPlates)
	{
		if (::IsValid(Plate))
		{
			Plate->ResetGimmick();
		}
	}
}

void AS1GimmickPuzzle_Sequence::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SpawnPlates();
	}
}

void AS1GimmickPuzzle_Sequence::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		for (AS1PressurePlate_Gimmick* Plate : SpawnedPlates)
		{
			if (::IsValid(Plate))
			{
				Plate->OnPlateTriggered.RemoveDynamic(this, &ThisClass::HandlePlateTriggered);
				Plate->Destroy();
			}
		}
		SpawnedPlates.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

void AS1GimmickPuzzle_Sequence::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	for (UBillboardComponent* Marker : PreviewMarkers)
	{
		if (::IsValid(Marker))
		{
			Marker->DestroyComponent();
		}
	}
	PreviewMarkers.Reset();

	for (const FVector& Offset : PlateOffsets)
	{
		UBillboardComponent* Marker = NewObject<UBillboardComponent>(this, NAME_None, RF_Transient);
		Marker->SetupAttachment(SceneRoot);
		Marker->SetRelativeLocation(Offset);
		Marker->SetHiddenInGame(true);
		Marker->RegisterComponent();
		PreviewMarkers.Add(Marker);
	}
#endif
}

void AS1GimmickPuzzle_Sequence::OnRep_IsActivated()
{
	Super::OnRep_IsActivated();

	// 완료 전환일 때만 연출(리셋 전환에는 아무 것도 하지 않음). 서버 호출·클라 OnRep 양쪽에서 로컬로 재생.
	if (false == IsUsable())
	{
		if (US1EffectData* EffectData = US1AssetManager::GetAssetByTag<US1EffectData>(S1AssetTags::Asset_Effect))
		{
			if (TSubclassOf<AS1Effect> EffectClass = EffectData->FindEffectClassByTag(S1EffectTags::Effect_Particle_Dust))
			{
				EffectClass->GetDefaultObject<AS1Effect>()->PlayEffect(GetWorld(), GetActorLocation(), FRotator::ZeroRotator);
			}
		}
	}
}

void AS1GimmickPuzzle_Sequence::HandlePlateTriggered(AS1PressurePlate_Gimmick* Plate)
{
	if (false == HasAuthority() || false == IsUsable())
	{
		return;
	}

	if (false == SpawnedPlates.IsValidIndex(CurrentStepIndex))
	{
		return;
	}

	if (SpawnedPlates[CurrentStepIndex] == Plate)
	{
		++CurrentStepIndex;

		if (CurrentStepIndex >= SpawnedPlates.Num())
		{
			ActivateGimmick();
			OnPuzzleCompleted.Broadcast();
		}
	}
	else
	{
		// 오답: 발판/인덱스 전부 초기화(외부 GimmickManager->ResetGimmicks(Tag)와 동일 경로).
		ResetGimmick();
	}
}

void AS1GimmickPuzzle_Sequence::SpawnPlates()
{
	UWorld* World = GetWorld();
	if (nullptr == World || nullptr == PlateClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	for (const FVector& Offset : PlateOffsets)
	{
		const FVector SpawnLocation = GetActorTransform().TransformPosition(Offset);
		AS1PressurePlate_Gimmick* Plate = World->SpawnActor<AS1PressurePlate_Gimmick>(PlateClass, SpawnLocation, GetActorRotation(), SpawnParams);
		if (nullptr == Plate)
		{
			continue;
		}

		Plate->OnPlateTriggered.AddDynamic(this, &ThisClass::HandlePlateTriggered);
		SpawnedPlates.Add(Plate);
	}
}
