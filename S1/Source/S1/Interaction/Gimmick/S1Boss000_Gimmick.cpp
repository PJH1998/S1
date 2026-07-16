// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/Gimmick/S1Boss000_Gimmick.h"

#include "Components/StaticMeshComponent.h"
#include "Character/S1Monster.h"
#include "S1Define.h"
#include "TimerManager.h"
#include "System/S1AssetManager.h"
#include "Data/S1EffectData.h"
#include "Effect/S1Effect.h"
#include "Tags/S1GameplayTags.h"

AS1Boss000_Gimmick::AS1Boss000_Gimmick()
{
	IntactMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IntactMesh"));
	SetRootComponent(IntactMesh);
	// 보스가 올라서는 발판(블록) + 보스 공격 콜리전(CC_Weapon) 오버랩으로 파괴 감지.
	// 보스 공격 콜리전 프로파일이 이 기둥의 오브젝트 채널을 Overlap 하도록 에디터에서 함께 설정해야 한다.
	IntactMesh->SetGenerateOverlapEvents(true);
	IntactMesh->SetCollisionResponseToChannel(S1CollisionChannel::CC_Weapon, ECR_Overlap);

	BreakMesh01 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BreakMesh01"));
	BreakMesh01->SetupAttachment(IntactMesh);
	BreakMesh01->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BreakMesh02 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BreakMesh02"));
	BreakMesh02->SetupAttachment(IntactMesh);
	BreakMesh02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AS1Boss000_Gimmick::BeginPlay()
{
	Super::BeginPlay();

	// 파편 초기 상대 트랜스폼 캡처(리셋 복원용). 분리 전에 1회.
	if (BreakMesh01)
	{
		BreakMesh01InitialRelative = BreakMesh01->GetRelativeTransform();
	}
	if (BreakMesh02)
	{
		BreakMesh02InitialRelative = BreakMesh02->GetRelativeTransform();
	}

	// 현재 복제 상태에 맞춰 초기 시각 동기화(late-join 클라 포함).
	ApplyBrokenVisual(false == IsUsable());

	if (HasAuthority())
	{
		IntactMesh->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnIntactMeshBeginOverlap);
	}
}

void AS1Boss000_Gimmick::OnRep_IsActivated()
{
	Super::OnRep_IsActivated();   // OnActivatedCosmetic 브로드캐스트

	// 전환 지점(서버 ActivateGimmick/ResetGimmick·클라 OnRep 공통). 파편 물리는 각 머신에서 로컬로 연출.
	if (false == IsUsable())
	{
		ApplyBrokenVisual(true);
		PlayBreakFx();
	}
	else
	{
		RestoreFragments();
		ApplyBrokenVisual(false);
	}
}

void AS1Boss000_Gimmick::OnIntactMeshBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (false == HasAuthority())
	{
		return;
	}

	if (false == IsUsable() || false == ::IsValid(OtherActor) || nullptr == OtherComp)
	{
		return;
	}

	// 보스(몬스터)의 공격 콜리전(CC_Weapon)만 인정 — 캡슐/바디나 다른 액터는 무시.
	if (false == OtherActor->IsA(AS1Monster::StaticClass()))
	{
		return;
	}

	if (OtherComp->GetCollisionObjectType() != S1CollisionChannel::CC_Weapon)
	{
		return;
	}

	// 특정 공격만 부수려면 AcceptedBreakTags로 해당 공격 콜리전 컴포넌트 태그를 게이팅.
	if (false == AcceptedBreakTags.IsEmpty())
	{
		bool bAccepted = false;
		for (const FGameplayTag& BreakTag : AcceptedBreakTags)
		{
			if (OtherComp->ComponentHasTag(BreakTag.GetTagName()))
			{
				bAccepted = true;
				break;
			}
		}

		if (false == bAccepted)
		{
			return;
		}
	}

	ActivateGimmick();
}

void AS1Boss000_Gimmick::ApplyBrokenVisual(bool bBroken)
{
	if (IntactMesh)
	{
		IntactMesh->SetVisibility(false == bBroken);
		IntactMesh->SetCollisionEnabled(bBroken ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
	}

	if (BreakMesh01)
	{
		BreakMesh01->SetVisibility(bBroken);
	}

	if (BreakMesh02)
	{
		BreakMesh02->SetVisibility(bBroken);
	}
}

FVector AS1Boss000_Gimmick::GetJumpTopLocation() const
{
	if (nullptr == IntactMesh)
	{
		return GetActorLocation();
	}

	if (false == JumpTopSocketName.IsNone() && IntactMesh->DoesSocketExist(JumpTopSocketName))
	{
		return IntactMesh->GetSocketLocation(JumpTopSocketName);
	}

	const FBoxSphereBounds MeshBounds = IntactMesh->Bounds;
	return FVector(MeshBounds.Origin.X, MeshBounds.Origin.Y, MeshBounds.Origin.Z + MeshBounds.BoxExtent.Z);
}

void AS1Boss000_Gimmick::PlayBreakFx()
{
	const FVector Center = GetActorLocation();

	if (US1EffectData* EffectData = US1AssetManager::GetAssetByTag<US1EffectData>(S1AssetTags::Asset_Effect))
	{
		if (TSubclassOf<AS1Effect> EffectClass = EffectData->FindEffectClassByTag(S1EffectTags::Effect_Particle_Dust))
		{
			EffectClass->GetDefaultObject<AS1Effect>()->PlayEffect(GetWorld(), Center, FRotator::ZeroRotator);
		}
	}

	UStaticMeshComponent* Fragments[] = { BreakMesh01, BreakMesh02 };
	for (UStaticMeshComponent* Fragment : Fragments)
	{
		if (nullptr == Fragment)
		{
			continue;
		}

		Fragment->SetVisibility(true);
		Fragment->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		// 바닥(WorldStatic)에만 부딪히고 캐릭터/공격 콜리전은 무시 — 파편이 무언가를 밀지 않게.
		Fragment->SetCollisionObjectType(ECC_WorldDynamic);
		Fragment->SetCollisionResponseToAllChannels(ECR_Ignore);
		Fragment->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		Fragment->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Fragment->SetSimulatePhysics(true);

		FVector Outward = Fragment->GetComponentLocation() - Center;
		Outward.Z = 0.f;
		if (false == Outward.Normalize())
		{
			Outward = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), 0.f).GetSafeNormal();
		}

		const FVector Impulse = Outward * FragmentImpulseStrength + FVector(0.f, 0.f, FragmentUpBias);
		Fragment->AddImpulse(Impulse, NAME_None, true);   // bVelChange=true → 질량 무관(속도 변화)
	}

	if (FragmentLifetime > 0.f)
	{
		GetWorldTimerManager().SetTimer(FragmentTimerHandle, this, &AS1Boss000_Gimmick::OnFragmentLifetimeExpired, FragmentLifetime, false);
	}
}

void AS1Boss000_Gimmick::OnFragmentLifetimeExpired()
{
	UStaticMeshComponent* Fragments[] = { BreakMesh01, BreakMesh02 };
	for (UStaticMeshComponent* Fragment : Fragments)
	{
		if (nullptr == Fragment)
		{
			continue;
		}

		Fragment->SetSimulatePhysics(false);
		Fragment->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Fragment->SetVisibility(false);
	}
}

void AS1Boss000_Gimmick::RestoreFragments()
{
	GetWorldTimerManager().ClearTimer(FragmentTimerHandle);

	if (BreakMesh01)
	{
		BreakMesh01->SetSimulatePhysics(false);
		BreakMesh01->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BreakMesh01->AttachToComponent(IntactMesh, FAttachmentTransformRules::KeepWorldTransform);
		BreakMesh01->SetRelativeTransform(BreakMesh01InitialRelative);
		BreakMesh01->SetVisibility(false);
	}

	if (BreakMesh02)
	{
		BreakMesh02->SetSimulatePhysics(false);
		BreakMesh02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BreakMesh02->AttachToComponent(IntactMesh, FAttachmentTransformRules::KeepWorldTransform);
		BreakMesh02->SetRelativeTransform(BreakMesh02InitialRelative);
		BreakMesh02->SetVisibility(false);
	}
}
