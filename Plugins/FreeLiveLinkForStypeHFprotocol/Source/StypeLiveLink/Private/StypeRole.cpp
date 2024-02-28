// Copyright Epic Games, Inc. All Rights Reserved.

#include "StypeRole.h"
#include "StypeTypes.h"
#include "Roles/LiveLinkCameraTypes.h"

#define LOCTEXT_NAMESPACE "StypeLiveLinkRole"

UScriptStruct* ULiveLinkStypeRole::GetStaticDataStruct() const
{
	return FStypeLiveLinkStaticData::StaticStruct();
}

UScriptStruct* ULiveLinkStypeRole::GetFrameDataStruct() const
{
	return FStypeLiveLinkFrameData::StaticStruct();
}

UScriptStruct* ULiveLinkStypeRole::GetBlueprintDataStruct() const
{
	return FStypeLiveLinkBlueprintData::StaticStruct();
}

bool ULiveLinkStypeRole::InitializeBlueprintData(const FLiveLinkSubjectFrameData& InSourceData, FLiveLinkBlueprintDataStruct& OutBlueprintData) const
{
	bool bSuccess = false;

	FStypeLiveLinkBlueprintData* BlueprintData = OutBlueprintData.Cast<FStypeLiveLinkBlueprintData>();
	const FStypeLiveLinkStaticData* StaticData = InSourceData.StaticData.Cast<FStypeLiveLinkStaticData>();
	const FStypeLiveLinkFrameData* FrameData = InSourceData.FrameData.Cast<FStypeLiveLinkFrameData>();
	if (BlueprintData && StaticData && FrameData)
	{
		GetStaticDataStruct()->CopyScriptStruct(&BlueprintData->StypeStaticData, StaticData);
		GetFrameDataStruct()->CopyScriptStruct(&BlueprintData->StypeFrameData, FrameData);
		bSuccess = true;
	}

	return bSuccess;
}

FText ULiveLinkStypeRole::GetDisplayName() const
{
	return LOCTEXT("StypeRole", "Stype Camera");
}

#undef LOCTEXT_NAMESPACE
