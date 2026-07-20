// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/S1InteractComponent.h"

#include "Components/SphereComponent.h"
#include "Interface/S1InteractableInterface.h"
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

	// 이미 범위 안에 있는 상호작용 액터 수동 등록 (BeginPlay 시점에는 Overlap 이벤트 미발화)
	TArray<AActor*> AlreadyOverlapping;
	InteractSphere->GetOverlappingActors(AlreadyOverlapping);
	for (AActor* Actor : AlreadyOverlapping)
	{
		if (Actor && Actor->Implements<US1InteractableInterface>())
		{
			CandidateSet.Add(Actor);
		}
	}

	UpdateNearestInteractable();
}

AActor* US1InteractComponent::GetNearestInteractable() const
{
	AActor* Owner = GetOwner();
	if (false == ::IsValid(Owner))
	{
		return nullptr;
	}

	AActor* Nearest = nullptr;
	float NearestDistSq = TNumericLimits<float>::Max();

	for (const TWeakObjectPtr<AActor>& Weak : CandidateSet)
	{
		AActor* Candidate = Weak.Get();
		IS1InteractableInterface* Interactable = Cast<IS1InteractableInterface>(Candidate);
		if (nullptr == Interactable || false == Interactable->CanInteract())
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(Owner->GetActorLocation(), Candidate->GetActorLocation());
		if (DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			Nearest = Candidate;
		}
	}

	return Nearest;
}

void US1InteractComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (nullptr == OtherActor || false == OtherActor->Implements<US1InteractableInterface>())
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
	AActor* NewNearest = GetNearestInteractable();
	if (NewNearest == CachedNearest.Get())
	{
		return;
	}

	CachedNearest = NewNearest;
	OnNearestInteractableChanged.Broadcast(NewNearest);
}
