// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "S1InteractableInterface.generated.h"

UINTERFACE(MinimalAPI)
class US1InteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class S1_API IS1InteractableInterface
{
	GENERATED_BODY()

public:
	virtual void Interact(AActor* Interactor) {}
	virtual bool CanInteract() const { return true; }
};
