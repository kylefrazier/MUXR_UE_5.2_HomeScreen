// Copyright Epic Games, Inc. All Rights Reserved.

#include "StypeTimingEstimator.h"

FStypeTimingEstimator::FStypeTimingEstimator()
	: LastPackage_no(0)
	, PacketsSinceLastTimecodeOverflow(0)
	, TimecodeRate(2,24)
	, FramesPerSecond(2,60)
{
	// We can't add higher rates because the stype protocol does not currently support it.
	TSet<uint8> ValidValues({24, 25, 30, 48, 50, 60, 72, 75, 96, 100, 120});
	
	TimecodeRate.SetValidValues(ValidValues);
	FramesPerSecond.SetValidValues(ValidValues);
}

void FStypeTimingEstimator::Reset()
{
	// Make no assumptions from the past. We reset all our estimations.
	TimecodeRate.Reset();
	FramesPerSecond.Reset();
}

int32 FStypeTimingEstimator::ElapsedPackage_no(uint8 A, uint8 B)
{
	// Examples:
	// Elapsed from   1 to 10 is 9.
	// Elapsed from 255 to  1 is 2.

	int32 Elapsed;
	
	if(B > A)
	{
		Elapsed = B - A;
	}
	else
	{
		Elapsed = ((255 - A) + B) + 1;
	}
	
	return Elapsed;
}

void FStypeTimingEstimator::InferTimecodeRate(const FTimecode& Timecode)
{
	TimecodeRate.Update(Timecode.Frames);
	
	// If timecode overflow happened, remember what it was.
	// It will be useful when calculating qualified frame time.
	if (TimecodeRate.Overflowed()) // Overflow is detected even before tcrate is known.
	{
		TimecodeAtLastOverflow = Timecode;
	}
}

void FStypeTimingEstimator::InferFramesPerSecond(uint8 Package_no)
{
	// Note: Allow this inference to run even if TimecodeRate is not valid to expedite the process.

	// Incrementally accumulate since Package_no overflows quickly.
	{
		int32 ElapsedPackets = ElapsedPackage_no(LastPackage_no, Package_no);
		PacketsSinceLastTimecodeOverflow += ElapsedPackets;
		VerifiedPacketsSinceLastTimecodeOverflow += ElapsedPackets;
	}

	// Only estimate frames per second immediately after a timecode FF overflow
	// because it means that a whole number of seconds have elapsed.
	if(!TimecodeRate.Overflowed())
	{
		return;
	}

	// Estimation of fps is based on how many packets have been received between timecode frame overflows (which happen every second).
	FramesPerSecond.Update(PacketsSinceLastTimecodeOverflow);
	
	// PacketsSinceLastTimecodeOverflow needs to be a multiple of FramesPerSecond (normally they are equal). 
	// Otherwise an overflow packet may have been missed.
	if(FramesPerSecond.IsValid() && !(PacketsSinceLastTimecodeOverflow % FramesPerSecond.GetEstimation()))
	{
		VerifiedTimecodeAtLastOverflow = TimecodeAtLastOverflow;
		VerifiedPacketsSinceLastTimecodeOverflow = 0;
	}
	
	// Because there was a new overflow, reset the count of packets.
	PacketsSinceLastTimecodeOverflow = 0;
}

void FStypeTimingEstimator::UpdateFrameTime(uint8 Package_no, const FTimecode& Timecode)
{
	// The idea here is to calculate how many packets have elapsed since the last time Timecode frames overflowed.
	// The current qualified frame time is the equal to that time overflow time plus the number of frames received since then.
	// It uses the packet_no to make this calculation immune to lost packets (unless consecutive lost packets exceed 255).

	FFrameRate FrameRate(FramesPerSecond.GetEstimation(), 1);

	// When using ToFrameNumber, note that Timecode.Frames is zero (because it is an overflow timecode), 
	// so using packet rate instead of timecode rate is ok.

	check(VerifiedTimecodeAtLastOverflow.Frames == 0)

	int32 NumFramesFullRate = VerifiedTimecodeAtLastOverflow.ToFrameNumber(FrameRate).Value;
	NumFramesFullRate += VerifiedPacketsSinceLastTimecodeOverflow;
	
	QualifiedFrameTime = FQualifiedFrameTime(FFrameTime(NumFramesFullRate), FrameRate);
}

void FStypeTimingEstimator::Update(uint8 Package_no, const FTimecode& Timecode)
{
	// Update inferenced values based on new packet timing info (packet no and timecode)
	InferTimecodeRate(Timecode);
	InferFramesPerSecond(Package_no);

	// Because there is a new packet, we need to update our estimation of time.
	UpdateFrameTime(Package_no, Timecode);
	
	// We need to remember this number so that we can accumulate the changes in PacketsSinceLastTimecodeOverflow.
	LastPackage_no = Package_no;
}

bool FStypeTimingEstimator::IsValid() const
{
	// Estimation can only be valid if we have valid estimations of timecode rate and fps.
	return TimecodeRate.IsValid() && FramesPerSecond.IsValid();
}

FQualifiedFrameTime FStypeTimingEstimator::GetQualifiedFrameTime() const
{
	// This value was cached in UpdateFrameTime(...)
	return QualifiedFrameTime;
}

void FStypeTimingEstimator::SetValidTimecodeRates(const TSet<uint8>& ValidValues)
{
	TimecodeRate.SetValidValues(ValidValues);
}

void FStypeTimingEstimator::SetValidFrameRates(const TSet<uint8>& ValidValues)
{
	FramesPerSecond.SetValidValues(ValidValues);
}

uint32 FStypeTimingEstimator::GetTimecodeRateEstimation()
{
	return TimecodeRate.GetEstimation();
}