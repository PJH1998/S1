// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/S1RespawnPillar.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Player/S1PlayerState.h"
#include "S1Define.h"

AS1RespawnPillar::AS1RespawnPillar()
{
	// 활성/비활성 페이드 + 왕복 오실레이션이 로컬 전용 비주얼이라 Tick 필요(BeginPlay에서 데디서버는 꺼둠).
	PrimaryActorTick.bCanEverTick = true;
	SetReplicateMovement(false);

	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarMesh"));
	SetRootComponent(PillarMesh);

	// S1PuzzleButton_Gimmick::InteractionCollision과 동일한 설정 — ObjectType 미설정으로 기본 WorldDynamic
	// 채널을 그대로 사용하고, US1InteractComponent의 감지 스피어(CC_Player)와 겹치도록 Overlap만 허용한다.
	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(PillarMesh);
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(S1CollisionChannel::CC_Player, ECR_Overlap);
	InteractionCollision->SetGenerateOverlapEvents(true);
}

void AS1RespawnPillar::Interact(AActor* Interactor)
{
	if (false == HasAuthority())
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(Interactor);
	AS1PlayerState* PS = ::IsValid(Pawn) ? Pawn->GetPlayerState<AS1PlayerState>() : nullptr;
	if (false == ::IsValid(PS))
	{
		return;
	}

	// 서버 권위 기준으로 재검증(CanInteract()는 상호작용을 요청한 클라이언트 자신의 로컬 판단이라 신뢰 불가) —
	// 이미 이 기둥이 활성 상태인 플레이어의 중복 상호작용(힐 파밍 등)을 막는다.
	if (PS->GetLastSpawnPointTag() == SpawnPointTag)
	{
		return;
	}

	PS->SetLastSpawnPointTag(SpawnPointTag);

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	US1AttributeSet* AttributeSet = ::IsValid(ASC) ? const_cast<US1AttributeSet*>(Cast<US1AttributeSet>(ASC->GetAttributeSet(US1AttributeSet::StaticClass()))) : nullptr;
	if (AttributeSet)
	{
		AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
	}
}

void AS1RespawnPillar::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_DedicatedServer)
	{
		SetActorTickEnabled(false); // 데디서버엔 화면이 없음 — 로컬 비주얼 로직(Tick 포함) 스킵
		return;
	}

	if (::IsValid(PillarMesh))
	{
		PillarMeshBaseRelativeLocation = PillarMesh->GetRelativeLocation();
	}

	BindLocalPlayerState();
}

void AS1RespawnPillar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TargetAlpha = bIsActivatedLocally ? 1.f : 0.f;
	CurrentActivatedAlpha = FMath::FInterpConstantTo(CurrentActivatedAlpha, TargetAlpha, DeltaTime, FadeSpeed);

	if (::IsValid(DynamicMaterial))
	{
		DynamicMaterial->SetScalarParameterValue(ActivatedParamName, CurrentActivatedAlpha);
	}

	if (false == ::IsValid(PillarMesh))
	{
		return;
	}

	if (bIsActivatedLocally)
	{
		OscillateElapsedTime += DeltaTime;
		const float ZOffset = FMath::Sin(OscillateElapsedTime * OscillateSpeed) * OscillateAmplitude;
		PillarMesh->SetRelativeLocation(PillarMeshBaseRelativeLocation + FVector(0.f, 0.f, ZOffset));
	}
	else if (false == PillarMesh->GetRelativeLocation().Equals(PillarMeshBaseRelativeLocation))
	{
		OscillateElapsedTime = 0.f;
		PillarMesh->SetRelativeLocation(PillarMeshBaseRelativeLocation);
	}
}

void AS1RespawnPillar::BindLocalPlayerState()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	AS1PlayerState* PS = ::IsValid(PC) ? PC->GetPlayerState<AS1PlayerState>() : nullptr;
	if (false == ::IsValid(PS))
	{
		// Possess/PlayerState 셋업 타이밍 레이스 — 다음 틱 재시도(#45 패턴).
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::BindLocalPlayerState);
		return;
	}

	BoundPlayerState = PS;
	PS->OnLastSpawnPointTagChanged.AddUObject(this, &ThisClass::OnLocalSpawnPointTagChanged);
	RefreshActivatedVisual();
}

void AS1RespawnPillar::OnLocalSpawnPointTagChanged(FGameplayTag NewTag)
{
	RefreshActivatedVisual();
}

void AS1RespawnPillar::RefreshActivatedVisual()
{
	// CanInteract()가 여기 의존하므로 머티리얼 유무와 무관하게 먼저 계산한다.
	bIsActivatedLocally = BoundPlayerState.IsValid() && BoundPlayerState->GetLastSpawnPointTag() == SpawnPointTag;

	// 실제 파라미터 페이드/오실레이션은 Tick이 CurrentActivatedAlpha 기준으로 매 프레임 처리 — 여기선 스냅하지 않는다.
	if (nullptr == DynamicMaterial && ::IsValid(PillarMesh))
	{
		if (UMaterialInterface* SourceMaterial = PillarMesh->GetMaterial(0))
		{
			DynamicMaterial = PillarMesh->CreateDynamicMaterialInstance(0, SourceMaterial);
		}
	}
}
