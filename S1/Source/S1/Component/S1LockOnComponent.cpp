#include "Component/S1LockOnComponent.h"
#include "Components/SphereComponent.h"
#include "Interface/S1LockOnInterface.h"
#include "Math/RotationMatrix.h"

US1LockOnComponent::US1LockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// CreateDefaultSubobject → 뷰포트에 표시 + BP에서 채널 설정 가능
	LockOnSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LockOnSphere"));
	LockOnSphere->InitSphereRadius(LockOnRadius);
	LockOnSphere->SetGenerateOverlapEvents(true);
}

void US1LockOnComponent::OnRegister()
{
	Super::OnRegister();

	// 에디터/런타임 둘 다 호출 → 루트에 부착해야 뷰포트에서 보임
	AActor* Owner = GetOwner();
	if (false == ::IsValid(Owner)) { return; }
	if (false == ::IsValid(LockOnSphere)) { return; }

	USceneComponent* Root = Owner->GetRootComponent();
	if (false == ::IsValid(Root)) { return; }

	LockOnSphere->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

void US1LockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	if (false == ::IsValid(LockOnSphere)) { return; }

	LockOnSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	LockOnSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);

	// 이미 범위 안에 있는 액터 수동 등록 (BeginPlay 시점에는 Overlap 이벤트 미발화)
	TArray<AActor*> AlreadyOverlapping;
	LockOnSphere->GetOverlappingActors(AlreadyOverlapping);
	for (AActor* Actor : AlreadyOverlapping)
	{
		if (Actor == GetOwner()) { continue; }
		if (false == Actor->Implements<US1LockOnInterface>()) { continue; }
		CandidateSet.Add(Actor);
	}
}

// ── 입력 ─────────────────────────────────────────────────────────────────────

void US1LockOnComponent::OnLockOnInput(const FRotator& CameraRotation)
{
	if (false == IsLockedOn())
	{
		BuildAndStartCycle(CameraRotation);
	}
	else
	{
		ReleaseLockOn();
	}
}

void US1LockOnComponent::CycleLeft(const FRotator& CameraRotation)
{
	RefreshCycleOrder(CameraRotation);
	if (CurrentIndex > 0)
	{
		--CurrentIndex;
	}
}

void US1LockOnComponent::CycleRight(const FRotator& CameraRotation)
{
	RefreshCycleOrder(CameraRotation);
	if (CurrentIndex < CycleList.Num() - 1)
	{
		++CurrentIndex;
	}
}

void US1LockOnComponent::ToggleCameraSide()
{
	CameraSide = (CameraSide == ES1LockOnSide::Right) ? ES1LockOnSide::Left : ES1LockOnSide::Right;
}

// ── 외부 조회 ────────────────────────────────────────────────────────────────

AActor* US1LockOnComponent::GetCurrentTarget() const
{
	if (false == CycleList.IsValidIndex(CurrentIndex))
	{
		return nullptr;
	}

	return CycleList[CurrentIndex].Target.Get();
}

FVector US1LockOnComponent::GetLockOnTargetLocation() const
{
	AActor* Target = GetCurrentTarget();
	if (false == ::IsValid(Target))
	{
		return FVector::ZeroVector;
	}

	if (Target->Implements<US1LockOnInterface>())
	{
		return IS1LockOnInterface::Execute_GetLockOnFocusLocation(Target);
	}

	return Target->GetActorLocation();
}

// ── 충돌 이벤트 ──────────────────────────────────────────────────────────────

void US1LockOnComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	if (false == OtherActor->Implements<US1LockOnInterface>())
	{
		return;
	}

	CandidateSet.Add(OtherActor);

	if (IsLockedOn())
	{
		InsertIntoActiveCycle(OtherActor);
	}
}

void US1LockOnComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CandidateSet.Remove(OtherActor);

	if (false == IsLockedOn()) { return; }

	if (GetCurrentTarget() == OtherActor)
	{
		ReleaseLockOn();
	}
	else
	{
		RemoveFromCycle(OtherActor);
	}
}

// ── 내부 로직 ────────────────────────────────────────────────────────────────

void US1LockOnComponent::BuildAndStartCycle(const FRotator& CameraRotation)
{
	CycleList.Empty();

	// 락온 시점의 카메라 기준 벡터 저장 (InsertIntoActiveCycle에서 재사용)
	const FVector CamFwd   = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::X);
	CycleForward2D = FVector(CamFwd.X, CamFwd.Y, 0.f).GetSafeNormal();
	CycleRight2D   = FVector::CrossProduct(FVector::UpVector, CycleForward2D).GetSafeNormal();

	for (const TWeakObjectPtr<AActor>& Weak : CandidateSet)
	{
		if (false == Weak.IsValid())
		{
			continue;
		}

		FS1LockOnEntry Entry  = MakeEntry(Weak.Get());
		Entry.SignedAngle      = ComputeSignedAngle(Weak.Get());
		CycleList.Add(Entry);
	}

	if (CycleList.IsEmpty())
	{
		return;
	}

	// 카메라 기준 왼쪽(-) → 오른쪽(+) 순 정렬
	CycleList.Sort([](const FS1LockOnEntry& A, const FS1LockOnEntry& B)
	{
		return A.SignedAngle < B.SignedAngle;
	});

	// 가장 정면(|각도| 최솟값)을 초기 타겟으로, 동률이면 거리 우선
	CurrentIndex      = 0;
	float MinAbsAngle = FMath::Abs(CycleList[0].SignedAngle);
	for (int32 i = 1; i < CycleList.Num(); ++i)
	{
		const float Abs = FMath::Abs(CycleList[i].SignedAngle);
		const bool bCloser = FMath::IsNearlyEqual(Abs, MinAbsAngle, 0.01f)
			&& CycleList[i].SnapshotDistance < CycleList[CurrentIndex].SnapshotDistance;

		if (Abs < MinAbsAngle || bCloser)
		{
			MinAbsAngle  = Abs;
			CurrentIndex = i;
		}
	}
}

void US1LockOnComponent::InsertIntoActiveCycle(AActor* NewActor)
{
	if (false == ::IsValid(NewActor)) { return; }

	FS1LockOnEntry NewEntry  = MakeEntry(NewActor);
	NewEntry.SignedAngle      = ComputeSignedAngle(NewActor);

	// 각도 순 정렬 유지하며 삽입 위치 탐색
	int32 InsertPos = CycleList.Num();
	for (int32 i = 0; i < CycleList.Num(); ++i)
	{
		if (NewEntry.SignedAngle < CycleList[i].SignedAngle)
		{
			InsertPos = i;
			break;
		}
	}

	CycleList.Insert(NewEntry, InsertPos);

	// 삽입 위치가 현재 타겟 이하면 CurrentIndex 보정
	if (InsertPos <= CurrentIndex)
	{
		++CurrentIndex;
	}
}

void US1LockOnComponent::RemoveFromCycle(AActor* Target)
{
	const int32 RemoveIdx = CycleList.IndexOfByPredicate(
		[Target](const FS1LockOnEntry& E) { return E.Target.Get() == Target; });

	if (RemoveIdx == INDEX_NONE) { return; }

	CycleList.RemoveAt(RemoveIdx);

	if (RemoveIdx < CurrentIndex)
	{
		--CurrentIndex;
	}

	if (CurrentIndex >= CycleList.Num() && false == CycleList.IsEmpty())
	{
		CurrentIndex = 0;
	}
}

void US1LockOnComponent::RefreshCycleOrder(const FRotator& CameraRotation)
{
	AActor* CurrentTarget = GetCurrentTarget();
	if (false == ::IsValid(CurrentTarget)) { ReleaseLockOn(); return; }

	// 카메라 기준 벡터 갱신
	const FVector CamFwd = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::X);
	CycleForward2D = FVector(CamFwd.X, CamFwd.Y, 0.f).GetSafeNormal();
	CycleRight2D   = FVector::CrossProduct(FVector::UpVector, CycleForward2D).GetSafeNormal();

	// 무효 엔트리 제거
	CycleList.RemoveAll([](const FS1LockOnEntry& E) { return !E.IsValid(); });
	if (CycleList.IsEmpty()) { ReleaseLockOn(); return; }

	// 각도 재계산 → 재정렬
	for (FS1LockOnEntry& Entry : CycleList)
	{
		Entry.SignedAngle = ComputeSignedAngle(Entry.Target.Get());
	}
	CycleList.Sort([](const FS1LockOnEntry& A, const FS1LockOnEntry& B)
	{
		return A.SignedAngle < B.SignedAngle;
	});

	// 재정렬 후 현재 타겟 인덱스 복원
	CurrentIndex = CycleList.IndexOfByPredicate([CurrentTarget](const FS1LockOnEntry& E)
	{
		return E.Target.Get() == CurrentTarget;
	});

	if (CurrentIndex == INDEX_NONE) { ReleaseLockOn(); }
}

void US1LockOnComponent::ReleaseLockOn()
{
	CurrentIndex   = INDEX_NONE;
	CycleList.Empty();
	CycleForward2D = FVector::ForwardVector;
	CycleRight2D   = FVector::RightVector;
}

FS1LockOnEntry US1LockOnComponent::MakeEntry(AActor* Actor) const
{
	FS1LockOnEntry Entry;
	Entry.Target           = Actor;
	Entry.SnapshotDistance = FVector::Dist(GetOwner()->GetActorLocation(), Actor->GetActorLocation());
	return Entry;
}

float US1LockOnComponent::ComputeSignedAngle(AActor* Actor) const
{
	const FVector ToTarget = (Actor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal2D();
	const float   FwdDot   = FVector::DotProduct(ToTarget, CycleForward2D);
	const float   SideDot  = FVector::DotProduct(ToTarget, CycleRight2D);
	return FMath::Atan2(SideDot, FwdDot);
}
