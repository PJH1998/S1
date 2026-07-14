// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/Gimmick/S1PuzzleButton_Gimmick.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Interaction/Gimmick/S1TogglePillar_Gimmick.h"
#include "S1Define.h"

AS1PuzzleButton_Gimmick::AS1PuzzleButton_Gimmick()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicateMovement(false);

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	SetRootComponent(ButtonMesh);

	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(ButtonMesh);
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(S1CollisionChannel::CC_Player, ECR_Overlap);
	InteractionCollision->SetGenerateOverlapEvents(true);
}

void AS1PuzzleButton_Gimmick::SetConnectedPillars(const TArray<AS1TogglePillar_Gimmick*>& InPillars)
{
	ConnectedPillars.Reset(InPillars.Num());
	for (AS1TogglePillar_Gimmick* Pillar : InPillars)
	{
		ConnectedPillars.Add(Pillar);
	}
}

void AS1PuzzleButton_Gimmick::SetPuzzleLocked(bool bLocked)
{
	bPuzzleLocked = bLocked;
}

void AS1PuzzleButton_Gimmick::PressButton()
{
	if (false == HasAuthority() || bPuzzleLocked)
	{
		return;
	}

	for (AS1TogglePillar_Gimmick* Pillar : ConnectedPillars)
	{
		if (false == ::IsValid(Pillar))
		{
			continue;
		}

		if (Pillar->IsUsable())
		{
			Pillar->ActivateGimmick();
		}
		else
		{
			Pillar->ResetGimmick();
		}
	}

	// 퍼즐 판정과 무관한 레버 눌림/복귀 연출용 토글.
	if (IsUsable())
	{
		ActivateGimmick();
	}
	else
	{
		ResetGimmick();
	}

	OnButtonToggled.Broadcast(this);
}