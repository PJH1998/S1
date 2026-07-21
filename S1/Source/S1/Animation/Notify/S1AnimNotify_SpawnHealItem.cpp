// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_SpawnHealItem.h"
#include "Animation/S1EffectSpawnLibrary.h"
#include "Character/Player/S1Player.h"
#if WITH_EDITOR
#include "PrimitiveDrawingUtils.h"
#endif

void US1AnimNotify_SpawnHealItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp)
	{
		return;
	}

	AS1Player* Player = Cast<AS1Player>(MeshComp->GetOwner());
	if (nullptr == Player)
	{
		return;
	}

	Player->SpawnHealItem(HealItemClass, Hand, AttachOffset, RotationRate, DissolveDuration);
}

#if WITH_EDITOR
void US1AnimNotify_SpawnHealItem::DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const
{
	if (nullptr == PDI || nullptr == MeshComp)
	{
		return;
	}

	// 프리뷰엔 Player 컨텍스트가 없어 실제 WeaponSocketName/OffhandSocketName(AS1Player) 대신 프로젝트 기본값 사용
	const FName SocketName = (Hand == ES1AttackHand::Offhand) ? TEXT("w_L_Attach") : TEXT("w_R_Attach");
	const FTransform SpawnTransform = S1EffectSpawnLibrary::ComputeSpawnTransform(MeshComp, SocketName, AttachOffset);

	DrawCoordinateSystem(PDI, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), 20.f, SDPG_Foreground);
	DrawWireSphere(PDI, SpawnTransform, FLinearColor::Yellow, 5.f, 12, SDPG_Foreground);
}
#endif
