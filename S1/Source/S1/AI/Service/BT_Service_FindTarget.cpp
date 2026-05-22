// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BT_Service_FindTarget.h"
#include "AI/S1AIController.h"
#include "Character/Player/S1Player.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/OverlapResult.h"

UBT_Service_FindTarget::UBT_Service_FindTarget()
	: Super()
{
	NodeName = TEXT("FindTargetService");
	Interval = 0.5f;
}

void UBT_Service_FindTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* LocalPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (LocalPawn == nullptr)
	{
		return;
	}

	UWorld* World = LocalPawn->GetWorld();
	if (World == nullptr)
	{
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	FVector Location = LocalPawn->GetActorLocation();
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, LocalPawn);

	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Location,
		FQuat::Identity,
		//ECollisionChannel::ECC_EngineTraceChannel1,
		ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(SearchRadius),
		CollisionQueryParam
	);

	if (bResult)
	{
		for (FOverlapResult& OverlapResult : OverlapResults)
		{
			AS1Player* Player = Cast<AS1Player>(OverlapResult.GetActor());
			if (Player)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, Player);
			}
		}
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, nullptr);
	}
}
