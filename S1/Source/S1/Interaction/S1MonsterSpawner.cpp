#include "Interaction/S1MonsterSpawner.h"

#include "Character/Player/S1Player.h"
#include "Character/S1Monster.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "S1Define.h"
#include "System/S1PoolingManager.h"

AS1MonsterSpawner::AS1MonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(SceneRoot);
	DetectionSphere->InitSphereRadius(DetectionRadius);
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(S1CollisionChannel::CC_Player, ECR_Overlap);
	DetectionSphere->SetGenerateOverlapEvents(true);
}

void AS1MonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (false == HasAuthority())
	{
		if (DetectionSphere)
		{
			DetectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			DetectionSphere->SetGenerateOverlapEvents(false);
		}
		return;
	}

	if (DetectionSphere)
	{
		DetectionSphere->SetSphereRadius(DetectionRadius);
		DetectionSphere->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnDetectionSphereBeginOverlap);
		DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnDetectionSphereBeginOverlap);
		DetectionSphere->OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::OnDetectionSphereEndOverlap);
		DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnDetectionSphereEndOverlap);
	}

	CacheAlreadyOverlappingPlayers();
	if (CanSpawnByRange())
	{
		ActivateSpawner();
	}
}

void AS1MonsterSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (DetectionSphere)
	{
		DetectionSphere->SetSphereRadius(DetectionRadius);
	}
}

void AS1MonsterSpawner::ActivateSpawner()
{
	if (false == HasAuthority())
	{
		return;
	}

	bHasActivated = true;
	SpawnAllAvailableSlots();
}

void AS1MonsterSpawner::OnDetectionSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (false == HasAuthority())
	{
		return;
	}

	if (IsPlayerActor(OtherActor) == false)
	{
		return;
	}

	OverlappingPlayers.Add(OtherActor);

	if (bHasActivated == false)
	{
		ActivateSpawner();
		return;
	}

	SpawnPendingSlots();
}

void AS1MonsterSpawner::OnDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (false == HasAuthority())
	{
		return;
	}

	if (IsPlayerActor(OtherActor))
	{
		OverlappingPlayers.Remove(OtherActor);
	}
}

void AS1MonsterSpawner::OnMonsterReturnedToPool(AS1Monster* Monster)
{
	if (false == HasAuthority())
	{
		return;
	}

	if (false == IsValid(Monster))
	{
		return;
	}
	
	Monster->OnReturnedToPool.RemoveDynamic(this, &ThisClass::OnMonsterReturnedToPool);

	for (int32 SlotIndex = 0; SlotIndex < SpawnSlots.Num(); ++SlotIndex)
	{
		FS1MonsterSpawnSlot& Slot = SpawnSlots[SlotIndex];
		if (Slot.ActiveMonster.Get() != Monster)
		{
			continue;
		}

		Slot.ActiveMonster.Reset();
		ScheduleRespawn(SlotIndex);
		return;
	}
}

void AS1MonsterSpawner::SpawnAllAvailableSlots()
{
	for (int32 SlotIndex = 0; SlotIndex < SpawnSlots.Num(); ++SlotIndex)
	{
		SpawnSlot(SlotIndex);
	}
}

void AS1MonsterSpawner::SpawnSlot(int32 SlotIndex)
{
	if (false == HasAuthority())
	{
		return;
	}

	if (SpawnSlots.IsValidIndex(SlotIndex) == false || CanSpawnByRange() == false)
	{
		return;
	}

	FS1MonsterSpawnSlot& Slot = SpawnSlots[SlotIndex];
	if (Slot.PoolTag.IsValid() == false || Slot.ActiveMonster.IsValid())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(Slot.RespawnTimerHandle);
	}

	US1PoolingManager* PoolingManager = GetWorld() ? GetWorld()->GetSubsystem<US1PoolingManager>() : nullptr;
	if (false == IsValid(PoolingManager))
	{
		return;
	}

	const FTransform SpawnTransform = GetSpawnTransform(Slot);
	AActor* SpawnedActor = PoolingManager->SpawnFromPool(Slot.PoolTag, SpawnTransform.GetLocation(), SpawnTransform.Rotator());
	AS1Monster* SpawnedMonster = Cast<AS1Monster>(SpawnedActor);
	if (false == IsValid(SpawnedMonster))
	{
		if (IsValid(SpawnedActor))
		{
			PoolingManager->ReturnToPool(SpawnedActor, Slot.PoolTag);
		}
		Slot.bPendingRespawn = true;
		ScheduleRespawn(SlotIndex);
		return;
	}

	Slot.bPendingRespawn = false;
	Slot.ActiveMonster = SpawnedMonster;
	SpawnedMonster->OnReturnedToPool.RemoveDynamic(this, &ThisClass::OnMonsterReturnedToPool);
	SpawnedMonster->OnReturnedToPool.AddDynamic(this, &ThisClass::OnMonsterReturnedToPool);
	SpawnedMonster->PlaySpawnAnimation();
}

void AS1MonsterSpawner::ScheduleRespawn(int32 SlotIndex)
{
	if (false == HasAuthority())
	{
		return;
	}

	if (SpawnSlots.IsValidIndex(SlotIndex) == false)
	{
		return;
	}

	FS1MonsterSpawnSlot& Slot = SpawnSlots[SlotIndex];
	const float Delay = Slot.bPendingRespawn ? PoolRetryTime : Slot.RespawnTime;
	if (Delay <= 0.f)
	{
		HandleRespawnTimerElapsed(SlotIndex);
		return;
	}

	if (UWorld* World = GetWorld())
	{
		FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::HandleRespawnTimerElapsed, SlotIndex);
		World->GetTimerManager().SetTimer(Slot.RespawnTimerHandle, TimerDelegate, Delay, false);
	}
}

void AS1MonsterSpawner::HandleRespawnTimerElapsed(int32 SlotIndex)
{
	if (false == HasAuthority())
	{
		return;
	}

	if (SpawnSlots.IsValidIndex(SlotIndex) == false)
	{
		return;
	}

	FS1MonsterSpawnSlot& Slot = SpawnSlots[SlotIndex];
	if (CanSpawnByRange() == false)
	{
		Slot.bPendingRespawn = true;
		return;
	}

	SpawnSlot(SlotIndex);
}

void AS1MonsterSpawner::SpawnPendingSlots()
{
	if (false == HasAuthority())
	{
		return;
	}

	for (int32 SlotIndex = 0; SlotIndex < SpawnSlots.Num(); ++SlotIndex)
	{
		if (SpawnSlots[SlotIndex].bPendingRespawn)
		{
			SpawnSlot(SlotIndex);
		}
	}
}

void AS1MonsterSpawner::CacheAlreadyOverlappingPlayers()
{
	if (DetectionSphere == nullptr)
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	DetectionSphere->GetOverlappingActors(OverlappingActors);
	for (AActor* Actor : OverlappingActors)
	{
		if (IsPlayerActor(Actor))
		{
			OverlappingPlayers.Add(Actor);
		}
	}
}

bool AS1MonsterSpawner::CanSpawnByRange() const
{
	if (bSpawnOnlyWhenPlayerInRange == false)
	{
		return true;
	}

	for (const TWeakObjectPtr<AActor>& Player : OverlappingPlayers)
	{
		if (Player.IsValid())
		{
			return true;
		}
	}

	return false;
}

bool AS1MonsterSpawner::IsPlayerActor(AActor* Actor) const
{
	return IsValid(Actor) && Actor->IsA<AS1Player>();
}

FTransform AS1MonsterSpawner::GetSpawnTransform(const FS1MonsterSpawnSlot& Slot) const
{
	if (IsValid(Slot.SpawnPoint))
	{
		return Slot.SpawnPoint->GetActorTransform();
	}

	return Slot.RelativeSpawnTransform * GetActorTransform();
}
