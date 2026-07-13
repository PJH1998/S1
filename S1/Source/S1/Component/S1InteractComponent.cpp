// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/S1InteractComponent.h"

#include "Components/SphereComponent.h"
#include "Interaction/Gimmick/S1PuzzleButton_Gimmick.h"

US1InteractComponent::US1InteractComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// CreateDefaultSubobject → 뷰포트에 표시 + BP에서 채널 설정 가능
	InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	InteractSphere->InitSphereRadius(InteractRadius);
	InteractSphere->SetGenerateOverlapEvents(true);
}

void US1InteractComponent::OnRegister()
{
	Super::OnRegister();

	// 에디터/런타임 둘 다 호출 → 루트에 부착해야 뷰포트에서 보임
	AActor* Owner = GetOwner();
	if (false == ::IsValid(Owner)) { return; }
	if (false == ::IsValid(InteractSphere)) { return; }

	USceneComponent* Root = Owner->GetRootComponent();
	if (false == ::IsValid(Root)) { return; }

	InteractSphere->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

void US1InteractComponent::BeginPlay()
{
	Super::BeginPlay();

	if (false == ::IsValid(InteractSphere)) { return; }

	InteractSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	InteractSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);

	// 이미 범위 안에 있는 버튼 수동 등록 (BeginPlay 시점에는 Overlap 이벤트 미발화)
	TArray<AActor*> AlreadyOverlapping;
	InteractSphere->GetOverlappingActors(AlreadyOverlapping, AS1PuzzleButton_Gimmick::StaticClass());
	for (AActor* Actor : AlreadyOverlapping)
	{
		CandidateSet.Add(Actor);
	}
}

AS1PuzzleButton_Gimmick* US1InteractComponent::GetNearestInteractable() const
{
	AActor* Owner = GetOwner();
	if (false == ::IsValid(Owner))
	{
		return nullptr;
	}

	AS1PuzzleButton_Gimmick* Nearest = nullptr;
	float NearestDistSq = TNumericLimits<float>::Max();

	for (const TWeakObjectPtr<AActor>& Weak : CandidateSet)
	{
		AS1PuzzleButton_Gimmick* Button = Cast<AS1PuzzleButton_Gimmick>(Weak.Get());
		if (nullptr == Button)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(Owner->GetActorLocation(), Button->GetActorLocation());
		if (DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			Nearest = Button;
		}
	}

	return Nearest;
}

void US1InteractComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (nullptr == Cast<AS1PuzzleButton_Gimmick>(OtherActor))
	{
		return;
	}

	CandidateSet.Add(OtherActor);
}

void US1InteractComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CandidateSet.Remove(OtherActor);
}