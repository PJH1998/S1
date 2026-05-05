// Fill out your copyright notice in the Description page of Project Settings.


#include "S1InputData.h"
#include "InputAction.h"
#include "InputMappingContext.h"

const UInputAction* US1InputData::FindInputActionByTag(const FGameplayTag& InputTag) const
{
	for (const FS1InputAction& Action : InputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	UE_LOG(LogWindows, Error, TEXT("Can't find InputAction for InputTag [%s]"), *InputTag.ToString());

	return nullptr;
}
