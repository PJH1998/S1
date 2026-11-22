// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/S1PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/AbilitySystemComponent/Player/S1PlayerAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "Component/S1InventoryComponent.h"
#include "Component/S1EquipComponent.h"

AS1PlayerState::AS1PlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// APlayerState 생성자는 SetNetUpdateFrequency(1) — 점수/핑처럼 가끔 바뀌는 데이터를 전제한 값이다.
	// 그런데 이 프로젝트는 ASC를 PlayerState에 두므로 몽타주 복제 정보(RepAnimMontageInfo), 어트리뷰트,
	// 게임플레이 태그가 전부 이 1Hz에 묶인다 → 다른 클라에서 몽타주가 최대 1초 늦게 보인다.
	// 엔진은 PlayMontageInternal에서 AvatarActor(Character)만 ForceNetUpdate 하므로("ASC가 아바타에
	// 붙어 있다"는 가정) PlayerState는 플러시되지 않고, CurrentMontageJumpToSection /
	// CurrentMontageSetNextSectionName은 ForceNetUpdate를 아예 하지 않는다.
	// 프로퍼티 복제는 변경분만 보내므로 주기를 올려도 유휴 시 비용은 거의 없다. 30은 데디 기본 틱.
	SetNetUpdateFrequency(30.f);

	AbilitySystemComponent = CreateDefaultSubobject<US1PlayerAbilitySystemComponent>("AbilitySystemComponent");
	PlayerSet = CreateDefaultSubobject<US1PlayerSet>("PlayerSet");
	InventoryComponent = CreateDefaultSubobject<US1InventoryComponent>("InventoryComponent");
	EquipComponent = CreateDefaultSubobject<US1EquipComponent>("EquipComponent");

	//TEMP
	FS1DefaultAttribute DefaultAttribute;
	DefaultAttribute.MaxHealth = 100.f;
	DefaultAttribute.MaxStamina = 100.f;
	DefaultAttribute.BaseDamage = 10.f;
	DefaultAttribute.BaseDefense = 10.f;

	PlayerSet->InitDefaultAttribute(DefaultAttribute);
}

UAbilitySystemComponent* AS1PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

US1AbilitySystemComponent* AS1PlayerState::GetS1AbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

US1PlayerSet* AS1PlayerState::GetS1PlayerSet() const
{
	return PlayerSet;
}

US1InventoryComponent* AS1PlayerState::GetInventoryComponent() const
{
	return InventoryComponent;
}

US1EquipComponent* AS1PlayerState::GetEquipComponent() const
{
	return EquipComponent;
}

void AS1PlayerState::InitPlayerSetFromTable(const FGameplayTag& AssetTag, const FGameplayTag& TableTag)
{
	if (!::IsValid(PlayerSet)) { return; }
	PlayerSet->InitAttributeFromTable(AssetTag, TableTag);
}

void AS1PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AS1PlayerState, SelectedCharacterTag);
	DOREPLIFETIME(AS1PlayerState, LastSpawnPointTag);
	DOREPLIFETIME(AS1PlayerState, SelectedWeaponTag);
}

void AS1PlayerState::SetSelectedCharacterTag(const FGameplayTag& InTag)
{
	SelectedCharacterTag = InTag;
}

void AS1PlayerState::SetLastSpawnPointTag(const FGameplayTag& InTag)
{
	LastSpawnPointTag = InTag;
	OnLastSpawnPointTagChanged.Broadcast(LastSpawnPointTag);
}

void AS1PlayerState::OnRep_LastSpawnPointTag()
{
	OnLastSpawnPointTagChanged.Broadcast(LastSpawnPointTag);
}

void AS1PlayerState::SetSelectedWeaponTag(const FGameplayTag& InTag)
{
	SelectedWeaponTag = InTag;
}

void AS1PlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (AS1PlayerState* S1PS = Cast<AS1PlayerState>(PlayerState))
	{
		S1PS->SelectedCharacterTag = SelectedCharacterTag;
		S1PS->LastSpawnPointTag = LastSpawnPointTag;
		S1PS->SelectedWeaponTag = SelectedWeaponTag;
	}
}

void AS1PlayerState::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	if (AS1PlayerState* S1PS = Cast<AS1PlayerState>(PlayerState))
	{
		SelectedCharacterTag = S1PS->SelectedCharacterTag;
		LastSpawnPointTag = S1PS->LastSpawnPointTag;
		SelectedWeaponTag = S1PS->SelectedWeaponTag;
	}
}
