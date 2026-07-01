#include "Character/S1SelectCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Data/S1CharacterSelectData.h"
#include "Data/S1WeaponData.h"
#include "Weapon/S1Weapon.h"
#include "Animation/Weapon/S1WeaponAnimLayer.h"
#include "System/S1AssetManager.h"
#include "Tags/S1GameplayTags.h"
#include "S1LogChannels.h"

AS1SelectCharacter::AS1SelectCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	SetRootComponent(BodyMesh);

	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HairMesh"));
	HairMesh->SetupAttachment(BodyMesh);
	HairMesh->SetLeaderPoseComponent(BodyMesh);

	FaceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FaceMesh"));
	FaceMesh->SetupAttachment(BodyMesh);
	FaceMesh->SetLeaderPoseComponent(BodyMesh);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(BodyMesh);
	SpringArm->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 90.f), FRotator(0.f, -90.f, 0.f));
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	PreviewCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("PreviewCapture"));
	PreviewCapture->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	PreviewCapture->bCaptureEveryFrame = true;
	PreviewCapture->bCaptureOnMovement = false;
	PreviewCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	PreviewCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
}

void AS1SelectCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (::IsValid(PreviewCapture))
	{
		PreviewCapture->TextureTarget = PreviewRenderTarget;
		PreviewCapture->ShowOnlyActors.Reset();
		PreviewCapture->ShowOnlyActors.Add(this);
	}
}

void AS1SelectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (false == bOrbitEnabled)
	{
		return;
	}

	FRotator Rot = SpringArm->GetRelativeRotation();
	Rot.Yaw += OrbitSpeed * DeltaTime;
	SpringArm->SetRelativeRotation(Rot);
}

void AS1SelectCharacter::ChangeMesh(const FGameplayTag& EntryTag)
{
	if (false == ::IsValid(CharacterSelectData))
	{
		LOG_WARNING(TEXT("ChangeMesh: CharacterSelectData is null"));
		return;
	}

	const FS1SelectCharacterEntry* Entry = CharacterSelectData->FindEntryByTag(EntryTag);
	if (nullptr == Entry)
	{
		LOG_WARNING(TEXT("ChangeMesh: no entry for tag [%s]"), *EntryTag.ToString());
		return;
	}

	CurrentGender = Entry->Gender;
	BodyMesh->SetSkeletalMeshAsset(Entry->BodyMesh);
	HairMesh->SetSkeletalMeshAsset(Entry->HairMesh);
	FaceMesh->SetSkeletalMeshAsset(Entry->FaceMesh);

	ChangeWeapon(Entry->DefaultWeaponTag);
}

void AS1SelectCharacter::ChangeWeapon(const FGameplayTag& WeaponTag)
{
	if (::IsValid(EquippedWeapon))
	{
		if (::IsValid(PreviewCapture))
		{
			PreviewCapture->ShowOnlyActors.Remove(EquippedWeapon);
		}
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}

	if (::IsValid(EquippedOffhandWeapon))
	{
		if (::IsValid(PreviewCapture))
		{
			PreviewCapture->ShowOnlyActors.Remove(EquippedOffhandWeapon);
		}
		EquippedOffhandWeapon->Destroy();
		EquippedOffhandWeapon = nullptr;
	}

	if (nullptr != CurrentAnimLayerClass)
	{
		BodyMesh->UnlinkAnimClassLayers(CurrentAnimLayerClass);
		CurrentAnimLayerClass = nullptr;
	}

	if (false == WeaponTag.IsValid())
	{
		return;
	}

	US1WeaponData* WeaponData = US1AssetManager::GetAssetByTag<US1WeaponData>(S1AssetTags::Asset_WeaponData);
	if (false == ::IsValid(WeaponData))
	{
		LOG_WARNING(TEXT("ChangeWeapon: WeaponData not loaded"));
		return;
	}

	const FS1WeaponEntry* Entry = WeaponData->FindEntryByTag(WeaponTag);
	if (nullptr == Entry)
	{
		LOG_WARNING(TEXT("ChangeWeapon: no entry for tag [%s]"), *WeaponTag.ToString());
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = this;

	if (nullptr != Entry->WeaponClass)
	{
		EquippedWeapon = GetWorld()->SpawnActor<AS1Weapon>(Entry->WeaponClass, Params);
		if (::IsValid(EquippedWeapon))
		{
			EquippedWeapon->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
			EquippedWeapon->SetActorRelativeRotation(FRotator(0.f, 0.f, -90.f));

			if (::IsValid(PreviewCapture))
			{
				PreviewCapture->ShowOnlyActors.AddUnique(EquippedWeapon);
			}
		}

		TSubclassOf<US1WeaponAnimLayer> LayerClass = Entry->WeaponClass->GetDefaultObject<AS1Weapon>()->GetAnimLayerClass(CurrentGender);
		if (nullptr == LayerClass)
		{
			LOG_WARNING(TEXT("ChangeWeapon: [%s] WeaponClass has no AnimLayerClass"), *WeaponTag.ToString());
		}
		else
		{
			BodyMesh->LinkAnimClassLayers(LayerClass);
			CurrentAnimLayerClass = LayerClass;
		}
	}

	if (nullptr != Entry->OffhandWeaponClass)
	{
		EquippedOffhandWeapon = GetWorld()->SpawnActor<AS1Weapon>(Entry->OffhandWeaponClass, Params);
		if (::IsValid(EquippedOffhandWeapon))
		{
			EquippedOffhandWeapon->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, OffhandSocketName);
			EquippedOffhandWeapon->SetActorRelativeRotation(FRotator(0.f, 0.f, -90.f));

			if (::IsValid(PreviewCapture))
			{
				PreviewCapture->ShowOnlyActors.AddUnique(EquippedOffhandWeapon);
			}
		}
	}
}

void AS1SelectCharacter::SetOrbitEnabled(bool bEnabled)
{
	bOrbitEnabled = bEnabled;
}

void AS1SelectCharacter::AddCameraYaw(float DeltaYaw)
{
	FRotator Rot = SpringArm->GetRelativeRotation();
	Rot.Yaw += DeltaYaw;
	SpringArm->SetRelativeRotation(Rot);
}

void AS1SelectCharacter::AddCameraPitch(float DeltaPitch)
{
	FRotator Rot = SpringArm->GetRelativeRotation();
	Rot.Pitch = FMath::Clamp(Rot.Pitch + DeltaPitch, PitchClamp.X, PitchClamp.Y);
	SpringArm->SetRelativeRotation(Rot);
}
