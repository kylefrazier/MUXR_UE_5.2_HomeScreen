// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "LiveLinkSourceSettings.h"

#include "Misc/FrameRate.h"
#include "UObject/ObjectMacros.h"

#include "StypeLiveLinkSourceSettings.generated.h"


UCLASS()
class STYPELIVELINK_API UStypeLiveLinkSourceSettings : public ULiveLinkSourceSettings
{
public:
	GENERATED_BODY()

	// Timecode FrameRate is detected as data comes in. Before it is correctly estimated, which timecode is expected
	UPROPERTY(EditAnywhere, Category = "Stype Settings")
	FFrameRate DefaultFrameRate = { 60, 1 };
};