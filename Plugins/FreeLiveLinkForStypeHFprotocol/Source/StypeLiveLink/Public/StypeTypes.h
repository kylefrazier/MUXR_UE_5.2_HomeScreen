// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "LiveLinkLensTypes.h"
#include "Roles/LiveLinkCameraTypes.h"
#include "StypeTypes.generated.h"

/**
 * Static data for Camera data.
 */
USTRUCT(BlueprintType)
struct STYPELIVELINK_API FStypeLiveLinkStaticData : public FLiveLinkLensStaticData
{
	GENERATED_BODY()

};

/**
 * Dynamic data for camera
 */
USTRUCT(BlueprintType)
struct STYPELIVELINK_API FStypeLiveLinkFrameData : public FLiveLinkLensFrameData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink")
	uint8 Command;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink")
	uint8 PacketNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink")
	int32 Frames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink")
	int32 Seconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink")
	int32 Minutes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink")
	int32 Hours;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink", Interp)
	float Focus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink", Interp)
	float Zoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink", Interp)
	float K1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink", Interp)
	float K2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink")
	FVector2D CenterShift;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink")
	float ProjectionAreaWidth;
};

/**
 * Facility structure to handle camera data in blueprint
 */
USTRUCT(BlueprintType)
struct STYPELIVELINK_API FStypeLiveLinkBlueprintData : public FLiveLinkLensBlueprintData
{
	GENERATED_BODY()

	// Static data that should not change every frame
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink")
	FStypeLiveLinkStaticData StypeStaticData;

	// Dynamic data that can change every frame
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink")
	FStypeLiveLinkFrameData StypeFrameData;
};
