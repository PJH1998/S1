// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/Gimmick/Sequence/S1GimmickPuzzle_TogglePillar.h"

#include "Components/BillboardComponent.h"
#include "Engine/World.h"
#include "Interaction/Gimmick/S1PuzzleButton_Gimmick.h"
#include "Interaction/Gimmick/S1TogglePillar_Gimmick.h"
#include "S1LogChannels.h"

namespace
{
	constexpr int32 MaxScrambleAttempts = 16;
}

AS1GimmickPuzzle_TogglePillar::AS1GimmickPuzzle_TogglePillar()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	PillarClass = AS1TogglePillar_Gimmick::StaticClass();
	ButtonClass = AS1PuzzleButton_Gimmick::StaticClass();
}

void AS1GimmickPuzzle_TogglePillar::BeginPlay()
{
	Super::BeginPlay();

	if (false == HasAuthority())
	{
		return;
	}

	// SpawnActor는 동기 호출 — 자식(기둥)의 BeginPlay(InitialLocation 캡처)가 이 시점에 이미
	// 끝나 있으므로, Sequence류가 겪는 "레벨에 미리 배치된 액터의 BeginPlay 순서 불확실성" 문제가
	// 애초에 발생하지 않는다(레벨 배치 참조 방식이었던 이전 안에서 필요했던 SetTimerForNextTick 불필요).
	SpawnPuzzleActors();
}

void AS1GimmickPuzzle_TogglePillar::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		for (AS1PuzzleButton_Gimmick* Button : SpawnedButtons)
		{
			if (::IsValid(Button))
			{
				Button->OnButtonToggled.RemoveDynamic(this, &ThisClass::HandleButtonToggled);
				Button->Destroy();
			}
		}
		SpawnedButtons.Reset();

		for (AS1TogglePillar_Gimmick* Pillar : SpawnedPillars)
		{
			if (::IsValid(Pillar))
			{
				Pillar->OnMovementFinished.RemoveDynamic(this, &ThisClass::HandlePillarMovementFinished);
				Pillar->Destroy();
			}
		}
		SpawnedPillars.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

void AS1GimmickPuzzle_TogglePillar::OnConstruction(const FTransform& Transform)
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

	for (const FVector& Offset : PillarOffsets)
	{
		UBillboardComponent* Marker = NewObject<UBillboardComponent>(this, NAME_None, RF_Transient);
		Marker->SetupAttachment(SceneRoot);
		Marker->SetRelativeLocation(Offset);
		Marker->SetHiddenInGame(true);
		Marker->RegisterComponent();
		PreviewMarkers.Add(Marker);
	}

	for (const FVector& Offset : ButtonOffsets)
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

void AS1GimmickPuzzle_TogglePillar::HandleButtonToggled(AS1PuzzleButton_Gimmick* Button)
{
	if (false == HasAuthority() || false == IsUsable())
	{
		return;
	}

	TryCompletePuzzle();
}

void AS1GimmickPuzzle_TogglePillar::HandlePillarMovementFinished()
{
	if (false == HasAuthority() || false == IsUsable())
	{
		return;
	}

	TryCompletePuzzle();
}

void AS1GimmickPuzzle_TogglePillar::TryCompletePuzzle()
{
	if (false == AreAllPillarsRaised())
	{
		return;
	}

	// 상태는 이미 정답이어도, 마지막으로 토글된 기둥이 아직 눈에 보이는 이동 중이면
	// 완료 처리를 미룬다 — 그 기둥의 OnMovementFinished가 다시 이 함수를 호출해줄 것이다.
	for (const AS1TogglePillar_Gimmick* Pillar : SpawnedPillars)
	{
		if (::IsValid(Pillar) && Pillar->IsMoving())
		{
			return;
		}
	}

	ActivateGimmick();

	for (AS1PuzzleButton_Gimmick* Button : SpawnedButtons)
	{
		if (::IsValid(Button))
		{
			Button->SetPuzzleLocked(true);
		}
	}

	OnPuzzleCompleted.Broadcast();
}

void AS1GimmickPuzzle_TogglePillar::SpawnPuzzleActors()
{
	UWorld* World = GetWorld();
	if (nullptr == World || nullptr == PillarClass || nullptr == ButtonClass)
	{
		return;
	}

	if (PillarOffsets.Num() != ButtonOffsets.Num() + 1)
	{
		LOG_WARNING(TEXT("AS1GimmickPuzzle_TogglePillar(%s): PillarOffsets(%d)는 ButtonOffsets(%d)+1개여야 체인 배선이 성립합니다."),
			*GetName(), PillarOffsets.Num(), ButtonOffsets.Num());
		return;
	}

	// 스크램블 결과(어떤 기둥 인덱스가 최저로 시작할지)를 스폰 전에 먼저 정한다 — 각 기둥이
	// BeginPlay의 순간 스냅에서 처음부터 정답 상태를 가리키게 하기 위함(스폰 후 애니메이션으로
	// 다시 내려가는 과도 상태가 생기지 않도록).
	const TSet<int32> LoweredIndices = ComputeScrambleLoweredIndices(ButtonOffsets.Num());

	for (int32 Index = 0; Index < PillarOffsets.Num(); ++Index)
	{
		const FTransform SpawnTransform(GetActorRotation(), GetActorTransform().TransformPosition(PillarOffsets[Index]));

		AS1TogglePillar_Gimmick* Pillar = World->SpawnActorDeferred<AS1TogglePillar_Gimmick>(PillarClass, SpawnTransform, this);
		if (nullptr == Pillar)
		{
			continue;
		}

		// FinishSpawning(→BeginPlay) 이전에 기준 위치와 최종 상태를 확정 — 순간 스냅이 바로 정답을 가리킨다.
		Pillar->SetSpawnBaseLocation(SpawnTransform.GetLocation());
		Pillar->SetInitialRaised(false == LoweredIndices.Contains(Index));
		Pillar->FinishSpawning(SpawnTransform);

		Pillar->OnMovementFinished.AddDynamic(this, &ThisClass::HandlePillarMovementFinished);
		SpawnedPillars.Add(Pillar);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	for (int32 Index = 0; Index < ButtonOffsets.Num(); ++Index)
	{
		if (false == SpawnedPillars.IsValidIndex(Index + 1))
		{
			continue;
		}

		const FVector SpawnLocation = GetActorTransform().TransformPosition(ButtonOffsets[Index]);
		AS1PuzzleButton_Gimmick* Button = World->SpawnActor<AS1PuzzleButton_Gimmick>(ButtonClass, SpawnLocation, GetActorRotation(), SpawnParams);
		if (nullptr == Button)
		{
			continue;
		}

		// 체인형 배선(GF(2) 선형독립 보장): 버튼 i ↔ 기둥 i, i+1.
		Button->SetConnectedPillars({ SpawnedPillars[Index], SpawnedPillars[Index + 1] });
		Button->OnButtonToggled.AddDynamic(this, &ThisClass::HandleButtonToggled);
		SpawnedButtons.Add(Button);
	}
}

TSet<int32> AS1GimmickPuzzle_TogglePillar::ComputeScrambleLoweredIndices(int32 ButtonCount) const
{
	if (ButtonCount <= 0)
	{
		return {};
	}

	const int32 ComboCount = 1 << ButtonCount;

	for (int32 Attempt = 0; Attempt < MaxScrambleAttempts; ++Attempt)
	{
		// 0(전부 안 누름)은 스크램블 없음이라 제외 — 최소 1개 버튼 조합을 역으로 눌러본다.
		const int32 Mask = FMath::RandRange(1, ComboCount - 1);

		TSet<int32> LoweredIndices;
		auto ToggleIndex = [&LoweredIndices](int32 PillarIndex)
		{
			if (LoweredIndices.Contains(PillarIndex))
			{
				LoweredIndices.Remove(PillarIndex);
			}
			else
			{
				LoweredIndices.Add(PillarIndex);
			}
		};

		for (int32 ButtonIndex = 0; ButtonIndex < ButtonCount; ++ButtonIndex)
		{
			if (0 == (Mask & (1 << ButtonIndex)))
			{
				continue;
			}

			// 체인 배선: 버튼 i ↔ 기둥 i, i+1.
			ToggleIndex(ButtonIndex);
			ToggleIndex(ButtonIndex + 1);
		}

		if (LoweredIndices.Num() > 0)
		{
			return LoweredIndices;
		}
	}

	return {};
}

bool AS1GimmickPuzzle_TogglePillar::AreAllPillarsRaised() const
{
	for (const AS1TogglePillar_Gimmick* Pillar : SpawnedPillars)
	{
		// IsUsable() == true는 "활성화(=최저 높이) 안 됨" = 최고 높이. 하나라도 최저면 미완료.
		if (false == ::IsValid(Pillar) || false == Pillar->IsUsable())
		{
			return false;
		}
	}

	return true;
}