// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/S1Character.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AS1Character::AS1Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AS1Character::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AS1Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAbilitySystemComponent* AS1Character::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AS1Character::InitSystem()
{
}

void AS1Character::MulticastPlayMontage_Implementation(UAnimMontage* Montage, float Rate, FName StartSection)
{
	if (nullptr == Montage)
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = GetMesh();
	UAnimInstance* AnimInst = IsValid(MeshComp) ? MeshComp->GetAnimInstance() : nullptr;
	if (nullptr == AnimInst)
	{
		return;
	}

	AnimInst->Montage_Play(Montage, Rate);

	if (StartSection != NAME_None)
	{
		AnimInst->Montage_JumpToSection(StartSection, Montage);
	}
}

void AS1Character::SetReplicatedGravityScale(float Scale)
{
	// 서버 권위 — 즉시 서버 CMC 적용 + 복제 트리거(소유 클라 포함 OnRep에서 적용)
	RepGravityScale = Scale;
	if (UCharacterMovementComponent* CMC = GetCharacterMovement())
	{
		CMC->GravityScale = Scale;
	}
}

void AS1Character::OnRep_GravityScale()
{
	if (UCharacterMovementComponent* CMC = GetCharacterMovement())
	{
		CMC->GravityScale = RepGravityScale;
	}
}

void AS1Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS1Character, RepGravityScale);
}
