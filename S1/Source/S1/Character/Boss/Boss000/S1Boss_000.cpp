// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/Boss000/S1Boss_000.h"

#include "AbilitySystem/Attributes/S1BossSet.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "Tags/S1GameplayTags.h"

#include "Component/S1DeathPresentationComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Rendering/SkeletalMeshRenderData.h"

namespace
{
	// 검/도끼가 둘 다 오른손이라 본을 접어 숨길 수 없으므로 머티리얼 섹션 단위로 숨긴다.
	// SKM_Boss000의 무기 머티리얼 슬롯 이름.
	const FName SwordMaterialSlot = TEXT("M_BOSS000_00_00_Sword");
	const FName AxeMaterialSlot   = TEXT("M_BOSS000_00_Ax");

	// 지정한 머티리얼 슬롯을 쓰는 LOD0 섹션들을 표시/숨김 처리한다.
	void SetWeaponSectionVisible(USkeletalMeshComponent* MeshComp, FName MaterialSlot, bool bVisible)
	{
		const int32 MaterialIndex = MeshComp->GetMaterialIndex(MaterialSlot);
		if (INDEX_NONE == MaterialIndex)
		{
			return;
		}

		FSkeletalMeshRenderData* RenderData = MeshComp->GetSkeletalMeshRenderData();
		if (nullptr == RenderData || false == RenderData->LODRenderData.IsValidIndex(0))
		{
			return;
		}

		const int32 LODIndex = 0;
		const TArray<FSkelMeshRenderSection>& Sections = RenderData->LODRenderData[LODIndex].RenderSections;
		for (int32 SectionIndex = 0; SectionIndex < Sections.Num(); ++SectionIndex)
		{
			if (Sections[SectionIndex].MaterialIndex == MaterialIndex)
			{
				MeshComp->ShowMaterialSection(MaterialIndex, SectionIndex, bVisible, LODIndex);
			}
		}
	}
}

AS1Boss_000::AS1Boss_000()
	: Super()
{
	// Collider
	GetCapsuleComponent()->SetCapsuleHalfHeight(400.f);
	GetCapsuleComponent()->SetCapsuleRadius(200.f);

	// Movement — Chase Notify가 yaw를 제어하므로 자동 회전은 비활성
	bUseControllerRotationYaw = false;
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = false;
	Movement->bUseControllerDesiredRotation = false;
	Movement->RotationRate.Yaw = 180.f;
	Movement->MaxWalkSpeed = DefaultMaxWalkSpeed;
	Movement->MaxAcceleration = DefaultMaxAcceleration;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -400.f), FRotator(0.f, -90.f, 0.f));

	// Shield — 왼손에 부착. 소켓 지정 없이 메쉬 루트에 붙이고, BeginPlay에서 Leader Pose로 본을 구동한다.
	// (메쉬 에셋은 BP_Boss_000에서 지정)
	ShieldMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ShieldMesh");
	ShieldMesh->SetupAttachment(GetMesh());
	// Leader Pose follower는 자체 포즈를 평가하지 않아 바운드가 손을 따라가지 못한다.
	// 부모(보스) 메쉬의 바운드를 그대로 써서 컬링으로 인한 깜빡임을 막는다.
	ShieldMesh->bUseAttachParentBound = true;

	// AttributeSet
	AttributeSet = CreateDefaultSubobject<US1BossSet>("AttributeSet");

	// UI
	BossName = TEXT("Cobalt");

	if (DeathPresentationComponent)
	{
		DeathPresentationComponent->PresentationMode = EDeathPresentation::AlphaFade;
		DeathPresentationComponent->FadeDuration = 2.f;
	}
}

void AS1Boss_000::BeginPlay()
{
	Super::BeginPlay();

	AttributeSet->InitAttributeFromTable(S1AssetTags::Asset_DataTable, S1DataTableTags::DataTable_MonsterData, TEXT("Boss000"));

	if (HasAuthority())
	{
		AbilitySystemComponent->AddCharacterAbilities(S1AssetTags::Asset_Ability_Boss000);
	}

	// 방패가 BP에서 지정된 경우 Leader Pose 연결 — 방패 w_L_Attach 본이 보스 손 w_L_Attach를 따라간다.
	if (ShieldMesh && ShieldMesh->GetSkeletalMeshAsset())
	{
		ShieldMesh->SetLeaderPoseComponent(GetMesh());
	}

	// 스폰 시 기본(도끼/1페이즈) 상태 적용. 복제된 ActiveWeapon은 OnRep으로 별도 반영된다.
	UpdateWeaponVisibility();
}

void AS1Boss_000::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS1Boss_000, ActiveWeapon);
	DOREPLIFETIME(AS1Boss_000, bWeaponHidden);
}

void AS1Boss_000::SetActiveWeapon(EBossWeapon NewWeapon)
{
	if (false == HasAuthority() || ActiveWeapon == NewWeapon)
	{
		return;
	}

	ActiveWeapon = NewWeapon;
	UpdateWeaponVisibility();   // OnRep은 서버에서 안 불리므로 서버에도 즉시 반영
	ForceNetUpdate();
}

void AS1Boss_000::SetWeaponHidden(bool bInHidden)
{
	if (false == HasAuthority() || bWeaponHidden == bInHidden)
	{
		return;
	}

	bWeaponHidden = bInHidden;
	UpdateWeaponVisibility();   // OnRep은 서버에서 안 불리므로 서버에도 즉시 반영
	ForceNetUpdate();
}

void AS1Boss_000::OnRep_ActiveWeapon()
{
	UpdateWeaponVisibility();
}

void AS1Boss_000::OnRep_WeaponHidden()
{
	UpdateWeaponVisibility();
}

void AS1Boss_000::UpdateWeaponVisibility()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (nullptr == MeshComp)
	{
		return;
	}

	// 페이즈 전환 던지기~뽑기 구간: 무기·방패 전부 숨김.
	const bool bShowAxe = (false == bWeaponHidden) && (ActiveWeapon == EBossWeapon::Axe);
	const bool bShowSword = (false == bWeaponHidden) && (ActiveWeapon == EBossWeapon::Sword);

	// 활성 무기는 표시, 비활성 무기는 숨긴다.
	SetWeaponSectionVisible(MeshComp, AxeMaterialSlot, bShowAxe);
	SetWeaponSectionVisible(MeshComp, SwordMaterialSlot, bShowSword);

	// 방패는 1페이즈(Axe)에서만 든다. 2페이즈(Sword)로 넘어가거나 숨김 구간이면 숨긴다.
	if (ShieldMesh)
	{
		ShieldMesh->SetVisibility(bShowAxe, /*bPropagateToChildren*/ true);
	}
}

void AS1Boss_000::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
