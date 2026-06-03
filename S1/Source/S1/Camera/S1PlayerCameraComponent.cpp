#include "Camera/S1PlayerCameraComponent.h"
#include "Component/S1LockOnComponent.h"
#include "Character/Player/S1Player.h"

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

		const FVector DesiredPos = PlayerPos
			- ToTarget               * BackDistance
			+ RightVec * (SideOffset * SideSign)
			+ FVector::UpVector      * HeightOffset;

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

US1LockOnComponent* US1PlayerCameraComponent::GetLockOnComponent() const
{
	const AS1Player* Player = Cast<AS1Player>(GetOwner());
	if (false == ::IsValid(Player)) { return nullptr; }
	return Player->GetLockOnComponent();
}
