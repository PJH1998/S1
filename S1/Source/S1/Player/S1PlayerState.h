// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTagContainer.h"
#include "S1PlayerState.generated.h"


class US1PlayerSet;
class US1AbilitySystemComponent;

UCLASS()
class S1_API AS1PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AS1PlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	US1AbilitySystemComponent* GetS1AbilitySystemComponent() const;
	US1PlayerSet* GetS1PlayerSet() const;
	void InitPlayerSetFromTable(const FGameplayTag& AssetTag, const FGameplayTag& TableTag);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1AbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<US1PlayerSet> PlayerSet;
};
