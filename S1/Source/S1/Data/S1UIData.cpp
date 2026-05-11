// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/S1UIData.h"

const TSubclassOf<US1BaseWidget> US1UIData::FindUserWidgetClassByTag(const FGameplayTag& UITag) const
{
	for (const FS1UserWidget& Widget : UserWidgets)
	{
		if (Widget.UserWidgetClass && Widget.UITag == UITag)
			return Widget.UserWidgetClass;
	}

	UE_LOG(LogWindows, Error, TEXT("Can't find UserWidgetClass for UITag [%s]"), *UITag.ToString());

	return nullptr;
}
