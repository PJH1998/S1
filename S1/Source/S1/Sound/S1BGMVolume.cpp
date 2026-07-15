// Fill out your copyright notice in the Description page of Project Settings.

#include "Sound/S1BGMVolume.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "System/S1SoundManager.h"

AS1BGMVolume::AS1BGMVolume()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(false);

	ZoneVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneVolume"));
	SetRootComponent(ZoneVolume);
	ZoneVolume->SetCollisionProfileName(TEXT("BGM_Volume"));
	ZoneVolume->SetGenerateOverlapEvents(true);
}

void AS1BGMVolume::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_DedicatedServer)
	{
		SetActorTickEnabled(false);
		return;
	}

	ZoneVolume->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnZoneBeginOverlap);
	ZoneVolume->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnZoneEndOverlap);
}

void AS1BGMVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 스폰 시점에 이미 이 볼륨 안이면 BeginOverlap이 발화하지 않으므로, 로컬 폰을 찾을 때까지만 직접 확인한다.
	APawn* LocalPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (false == IsValid(LocalPawn))
	{
		return;
	}

	if (IsLocationInsideZone(LocalPawn->GetActorLocation()))
	{
		RequestPlayBGM();
	}

	SetActorTickEnabled(false);
}

void AS1BGMVolume::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (false == IsValid(Pawn) || false == Pawn->IsLocallyControlled())
	{
		return;
	}

	// 경계에서 스치듯 왕복 시 재생이 반복되지 않도록, DebounceDuration 이상 머물러야 실제로 전환한다.
	GetWorld()->GetTimerManager().SetTimer(DebounceTimerHandle, this, &ThisClass::RequestPlayBGM, DebounceDuration, false);
}

void AS1BGMVolume::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (false == IsValid(Pawn) || false == Pawn->IsLocallyControlled())
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(DebounceTimerHandle);
}

void AS1BGMVolume::RequestPlayBGM()
{
	if (US1SoundManager* SoundManager = GetWorld()->GetSubsystem<US1SoundManager>())
	{
		SoundManager->PlayBGM(ZoneBGM, FadeDuration, Volume);
	}
}

bool AS1BGMVolume::IsLocationInsideZone(const FVector& Location) const
{
	return IsValid(ZoneVolume) && ZoneVolume->Bounds.GetBox().IsInside(Location);
}
