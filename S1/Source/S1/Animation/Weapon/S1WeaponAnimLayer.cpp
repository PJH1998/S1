#include "Animation/Weapon/S1WeaponAnimLayer.h"
#include "Animation/Player/S1PlayerAnimInstance.h"

US1PlayerAnimInstance* US1WeaponAnimLayer::GetPlayerAnimInstance() const
{
	USkeletalMeshComponent* Mesh = GetOwningComponent();
	if (false == ::IsValid(Mesh))
	{
		return nullptr;
	}

	return Cast<US1PlayerAnimInstance>(Mesh->GetAnimInstance());
}

void US1WeaponAnimLayer::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	US1PlayerAnimInstance* PlayerAnim = GetPlayerAnimInstance();
	if (false == ::IsValid(PlayerAnim))
	{
		return;
	}

	GroundSpeed   = PlayerAnim->GetGroundSpeed();
	bMove         = PlayerAnim->GetMove();
	bIsFalling    = PlayerAnim->GetIsFalling();
	bSprint       = PlayerAnim->GetSprint();
	bDirectToFall = PlayerAnim->GetDirectToFall();
	bWait         = PlayerAnim->GetWait();
	bRunLoop      = PlayerAnim->GetRunLoop();
}

