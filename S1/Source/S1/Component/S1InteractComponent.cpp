// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/S1InteractComponent.h"

#include "Components/SphereComponent.h"
#include "Interaction/Gimmick/S1PuzzleButton_Gimmick.h"
#include "S1Define.h"

US1InteractComponent::US1InteractComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// CreateDefaultSubobject → 뷰포트에 표시
	InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	InteractSphere->InitSphereRadius(InteractRadius);
	InteractSphere->SetGenerateOverlapEvents(true);

	// 순수 오버랩 트리거로 고정: Monster 채널을 Block하면 몬스터 캡슐과 겹칠 때 크게 튕겨나간다
	InteractSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractSphere->SetCollisionObjectType(S1CollisionChannel::CC_Player);
	InteractSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	// S1PuzzleButton_Gimmick::InteractionCollision은 ObjectType 미설정 → 기본 WorldDynamic 채널을 그대로 사용
	InteractSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
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

	UpdateNearestInteractable();
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
		if (nullptr == Button || Button->IsPuzzleLocked())
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
	UpdateNearestInteractable();
}

void US1InteractComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CandidateSet.Remove(OtherActor);
	UpdateNearestInteractable();
}

void US1InteractComponent::UpdateNearestInteractable()
{
	AS1PuzzleButton_Gimmick* NewNearest = GetNearestInteractable();
	if (NewNearest == CachedNearest.Get())
	{
		return;
	}

	CachedNearest = NewNearest;
	OnNearestInteractableChanged.Broadcast(NewNearest);
}
