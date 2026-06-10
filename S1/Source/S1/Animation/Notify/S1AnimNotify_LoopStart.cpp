#include "Animation/Notify/S1AnimNotify_LoopStart.h"
#include "Animation/Player/S1PlayerAnimInstance.h"

void US1AnimNotify_LoopStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (false == ::IsValid(MeshComp))
	{
		return;
	}

	US1PlayerAnimInstance* PlayerAnim = Cast<US1PlayerAnimInstance>(MeshComp->GetAnimInstance());
	if (::IsValid(PlayerAnim))
	{
		PlayerAnim->OnLoopStartNotify();
	}
}
