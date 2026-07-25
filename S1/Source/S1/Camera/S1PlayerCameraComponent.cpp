#include "Camera/S1PlayerCameraComponent.h"
#include "Component/S1LockOnComponent.h"
#include "Character/Player/S1Player.h"
#include "Engine/World.h"

US1PlayerCameraComponent::US1PlayerCameraComponent()
{
}

void US1PlayerCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	Super::GetCameraView(DeltaTime, DesiredView); // DesiredView = 스프링암 위치

	US1LockOnComponent* LockOnComp = GetLockOnComponent();
	const bool bIsLockedOn = LockOnComp && LockOnComp->IsLockedOn();

	if (bIsLockedOn)
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if (false == ::IsValid(OwnerPawn)) { return; }

		const FVector PlayerPos = OwnerPawn->GetActorLocation();
		const FVector TargetPos = LockOnComp->GetLockOnTargetLocation();
		const FVector ToTarget  = (TargetPos - PlayerPos).GetSafeNormal2D();

		if (ToTarget.IsNearlyZero()) { return; }

		const FVector RightVec = FVector::CrossProduct(FVector::UpVector, ToTarget).GetSafeNormal();
		const float   SideSign = (LockOnComp->GetCameraSide() == ES1LockOnSide::Right) ? 1.f : -1.f;

		// 카메라가 -ToTarget 방향으로 BackDistance만큼 뻗어나가는 기준점(피벗) — 스프링암 콜리전의 원점 역할
		const FVector Pivot = PlayerPos
			+ RightVec * (SideOffset * SideSign)
			+ FVector::UpVector * HeightOffset;
		const FVector DesiredPos = Pivot - ToTarget * BackDistance;

		const FVector  LookAt     = FMath::Lerp(PlayerPos, TargetPos, LookAtBlend);
		const FRotator DesiredRot = (LookAt - DesiredPos).Rotation();

		// 락온 첫 프레임: 스프링암 위치에서 보간 시작
		if (false == bWasLockedOn)
		{
			CurrentLockOnPos = DesiredView.Location;
			CurrentLockOnRot = DesiredView.Rotation;
			bWasLockedOn     = true;
			bIsBlendingOut   = false;
		}

		CurrentLockOnPos = FMath::VInterpTo(CurrentLockOnPos, DesiredPos, DeltaTime, PositionInterpSpeed);
		CurrentLockOnRot = FMath::RInterpTo(CurrentLockOnRot, DesiredRot, DeltaTime, RotationInterpSpeed);

		// 스프링암이 우회됐으므로 콜리전을 직접 재현 — 피벗→카메라 사이가 지오메트리에 막히면 앞으로 당김
		// (#1에서 Player/Monster 캡슐이 ECC_Camera를 Ignore하므로 다른 캐릭터는 통과, 월드만 걸림)
		CurrentLockOnPos = ApplyCameraCollision(Pivot, CurrentLockOnPos, OwnerPawn);

		DesiredView.Location = CurrentLockOnPos;
		DesiredView.Rotation = CurrentLockOnRot;
	}
	else
	{
		// 락온이 해제된 첫 프레임: 블렌드아웃 시작
		if (bWasLockedOn)
		{
			bWasLockedOn   = false;
			bIsBlendingOut = true;
		}

		if (bIsBlendingOut)
		{
			// DesiredView는 Super에서 스프링암 위치로 설정됨
			CurrentLockOnPos = FMath::VInterpTo(CurrentLockOnPos, DesiredView.Location, DeltaTime, PositionInterpSpeed);
			CurrentLockOnRot = FMath::RInterpTo(CurrentLockOnRot, DesiredView.Rotation, DeltaTime, RotationInterpSpeed);

			const bool bPosClose = FVector::Dist(CurrentLockOnPos, DesiredView.Location) < 5.f;
			const bool bRotClose = CurrentLockOnRot.Equals(DesiredView.Rotation, 0.5f);

			if (bPosClose && bRotClose)
			{
				bIsBlendingOut = false;
			}
			else
			{
				DesiredView.Location = CurrentLockOnPos;
				DesiredView.Rotation = CurrentLockOnRot;
			}
		}
	}
}

FVector US1PlayerCameraComponent::ApplyCameraCollision(const FVector& Pivot, const FVector& DesiredPos, AActor* IgnoreActor) const
{
	UWorld* World = GetWorld();
	if (false == ::IsValid(World))
	{
		return DesiredPos;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(LockOnCameraProbe), /*bTraceComplex=*/false, IgnoreActor);
	FHitResult Hit;
	const bool bBlocked = World->SweepSingleByChannel(
		Hit, Pivot, DesiredPos, FQuat::Identity, ProbeChannel,
		FCollisionShape::MakeSphere(ProbeSize), Params);

	// 스윕 구의 중심이 곧 충돌 시점 카메라 위치 — 프로브 반경만큼 이미 벽에서 떨어져 있음
	return bBlocked ? Hit.Location : DesiredPos;
}

US1LockOnComponent* US1PlayerCameraComponent::GetLockOnComponent() const
{
	const AS1Player* Player = Cast<AS1Player>(GetOwner());
	if (false == ::IsValid(Player)) { return nullptr; }
	return Player->GetLockOnComponent();
}
