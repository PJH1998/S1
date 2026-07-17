// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_SpawnPersistentEffect.h"
#include "Animation/S1EffectSpawnLibrary.h"
#include "Effect/NiagaraEffect/S1NiagaraEffect.h"
#include "Character/S1Character.h"
#include "Tags/S1GameplayTags.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#if WITH_EDITOR
#include "PrimitiveDrawingUtils.h"
#endif

US1AnimNotify_SpawnPersistentEffect::US1AnimNotify_SpawnPersistentEffect()
{
	AssetTag = S1AssetTags::Asset_Effect;
}

void US1AnimNotify_SpawnPersistentEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp)
	{
		return;
	}

#if WITH_EDITORONLY_DATA
	// 애님/몽타주 에디터 프리뷰에서만 — 무기/캐릭터 컨텍스트가 없으므로 항상 캐릭터 메시 소켓 기준
	if (::IsValid(PreviewNiagaraSystem) && nullptr != MeshComp->GetWorld() && EWorldType::EditorPreview == MeshComp->GetWorld()->WorldType)
	{
		if (bAttachToSocket)
		{
			UNiagaraComponent* PreviewComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
				PreviewNiagaraSystem,
				MeshComp,
				SpawnSocketName,
				SpawnOffset.GetLocation(),
				SpawnOffset.GetRotation().Rotator(),
				SpawnOffset.GetScale3D(),
				EAttachLocation::KeepRelativeOffset,
				true,
				ENCPoolMethod::AutoRelease,
				true);

			if (nullptr != PreviewComponent)
			{
				// Scale은 항상 절대값(부모 스케일 무시), Location/Rotation은 소켓 그대로 추종
				PreviewComponent->SetAbsolute(false, false, true);
				PreviewComponent->SetVariableLinearColor(ColorParameterName, InitialColor);
			}
		}
		else
		{
			const FTransform PreviewTransform = S1EffectSpawnLibrary::ComputeSpawnTransform(MeshComp, SpawnSocketName, SpawnOffset);
			UNiagaraComponent* PreviewComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				MeshComp->GetWorld(),
				PreviewNiagaraSystem,
				PreviewTransform.GetLocation(),
				PreviewTransform.GetRotation().Rotator(),
				PreviewTransform.GetScale3D(),
				true,
				true,
				ENCPoolMethod::AutoRelease);

			if (nullptr != PreviewComponent)
			{
				PreviewComponent->SetVariableLinearColor(ColorParameterName, InitialColor);
			}
		}
		return;
	}
#endif

	AS1Character* Character = Cast<AS1Character>(MeshComp->GetOwner());
	if (nullptr == Character)
	{
		return;
	}

	// 데디 서버는 렌더링 대상이 없어 스폰 자체가 낭비 — 스킵
	if (Character->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	AS1NiagaraEffect* NiagaraEffectCDO = S1EffectSpawnLibrary::FindNiagaraEffectCDO(AssetTag, EffectTag);
	if (nullptr == NiagaraEffectCDO)
	{
		return;
	}

	USkeletalMeshComponent* TargetMesh = S1EffectSpawnLibrary::ResolveTargetMesh(AttachTarget, Character, MeshComp);
	if (nullptr == TargetMesh)
	{
		return;
	}

	UNiagaraComponent* Component = nullptr;
	if (bAttachToSocket)
	{
		NiagaraEffectCDO->PlayEffectAttached(TargetMesh, SpawnSocketName, SpawnOffset, &Component);

		if (nullptr != Component)
		{
			// Scale은 항상 절대값(부모 스케일 무시), Location/Rotation은 소켓 그대로 추종
			Component->SetAbsolute(false, false, true);
		}
	}
	else
	{
		const FTransform SpawnTransform = S1EffectSpawnLibrary::ComputeSpawnTransform(TargetMesh, SpawnSocketName, SpawnOffset);
		NiagaraEffectCDO->PlayEffect(MeshComp->GetWorld(), SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), SpawnTransform.GetScale3D(), &Component);
	}

	if (nullptr != Component)
	{
		Component->SetVariableLinearColor(ColorParameterName, InitialColor);
		S1EffectSpawnLibrary::ApplyCustomDepthStencil(Component, bUseCustomDepth, CustomDepthStencilValue);
	}

	Character->RegisterAttachedEffect(EffectTag, Component);
}

#if WITH_EDITOR
void US1AnimNotify_SpawnPersistentEffect::DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const
{
	if (nullptr == PDI || nullptr == MeshComp)
	{
		return;
	}

	// 무기/캐릭터 컨텍스트가 없는 프리뷰 환경이므로 항상 캐릭터 메시 소켓 기준으로 계산
	const FTransform SpawnTransform = S1EffectSpawnLibrary::ComputeSpawnTransform(MeshComp, SpawnSocketName, SpawnOffset);
	DrawCoordinateSystem(PDI, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), 20.f, SDPG_Foreground);
	DrawWireSphere(PDI, SpawnTransform, FLinearColor::Yellow, 5.f, 12, SDPG_Foreground);
}
#endif
