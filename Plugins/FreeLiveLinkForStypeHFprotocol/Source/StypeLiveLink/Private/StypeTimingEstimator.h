// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/QualifiedFrameTime.h"
#include "StypeEstimationByAgreement.h"
#include "StypeInferredRateFromOverflow.h"

class FStypeTimingEstimator
{
	
	/** Value of the last Package_no given in Update()*/
	uint8 LastPackage_no;
	
	/** Number of packets since the last Timecode frames overflowed */
	uint32 PacketsSinceLastTimecodeOverflow;
	
	/** Timecode in packet of last overflow. By definition, frames of this Timecode is zero */
	FTimecode TimecodeAtLastOverflow;
	
	/** Number of packets since the last Timecode frames overflowed, verified to be correct based on elapsed packets and estimated frame rate */
	uint32 VerifiedPacketsSinceLastTimecodeOverflow;
	
	/** Timecode in packet of last overflow, verified to be correct based on elapsed packets and estimated frame rate*/
	FTimecode VerifiedTimecodeAtLastOverflow;
	
	/** Current estimation of QualifiedFrameTime */
	FQualifiedFrameTime QualifiedFrameTime;
	
	/** Timecode Rate estimator */
	FStypeInferredRateFromValueOverflow<uint8> TimecodeRate;
	
	/** Frames per second estimator */
	FStypeEstimationByAgreement<uint8> FramesPerSecond;
	
	/** Calculates elapsed Package_no's from Package_no A to Package_no B, accounting for wrap around from 255 to 0 */
	static int32 ElapsedPackage_no(uint8 A, uint8 B);
	
	/** Updates current estimation of QualifiedFrameTime based on Stype package timing fields, Package_no and Timecode */
	void UpdateFrameTime(uint8 Package_no, const FTimecode & Timecode);
	
	/** Infers the timecode rate */
	void InferTimecodeRate(const FTimecode& Timecode);

	/** Infers the fps of the samples */
	void InferFramesPerSecond(uint8 Package_no);

public:
	/** Constructor */
	FStypeTimingEstimator();
	
	/** Resets estimation */
	void Reset();
	
	/** Updates estimation with the timing parameters of the new Stype packet */
	void Update(uint8 Package_no, const FTimecode & Timecode);
	
	/** Returns current QualifiedFrameTime estimation */
	FQualifiedFrameTime GetQualifiedFrameTime() const;
	
	/** Returns true if a valid estimation has been reached. Calling Reset() will clear it */
	bool IsValid() const;

	/** Replaces default valid timecode rates */
	void SetValidTimecodeRates(const TSet<uint8>& ValidValues);

	/** Replaces default valid frame/sample rates */
	void SetValidFrameRates(const TSet<uint8>& ValidValues);

	/** Get TimeocodeRate estimation */
	uint32 GetTimecodeRateEstimation();
};


