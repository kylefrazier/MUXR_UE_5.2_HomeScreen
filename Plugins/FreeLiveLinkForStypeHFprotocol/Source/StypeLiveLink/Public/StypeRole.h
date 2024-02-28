// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "LiveLinkLensRole.h"
#include "Roles/LiveLinkCameraRole.h"
#include "StypeRole.generated.h"

/**
 * Role associated for Stype Camera data.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Stype Camera Role"))
class STYPELIVELINK_API ULiveLinkStypeRole : public ULiveLinkLensRole
{
	GENERATED_BODY()

public:
	virtual UScriptStruct* GetStaticDataStruct() const override;
	virtual UScriptStruct* GetFrameDataStruct() const override;
	virtual UScriptStruct* GetBlueprintDataStruct() const override;

	bool InitializeBlueprintData(const FLiveLinkSubjectFrameData& InSourceData, FLiveLinkBlueprintDataStruct& OutBlueprintData) const override;

	virtual FText GetDisplayName() const override;
};

