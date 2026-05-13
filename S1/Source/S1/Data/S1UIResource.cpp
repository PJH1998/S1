// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/S1UIResource.h"

UTexture2D* US1UIResource::FindTextureByTag(const FGameplayTag& ResourceTag) const
{
	for (const FS1UITexture& Resource : Resources)
	{
		if (Resource.Texture && Resource.ResourceTag == ResourceTag)
		{
			return Resource.Texture;
		}
	}

	UE_LOG(LogWindows, Error, TEXT("Can't find Texture for ResourceTag [%s]"), *ResourceTag.ToString());

	return nullptr;
}

