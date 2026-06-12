#include "Data/S1WeaponData.h"

const FS1WeaponEntry* US1WeaponData::FindEntryByTag(const FGameplayTag& ItemTag) const
{
	return WeaponEntries.Find(ItemTag);
}
