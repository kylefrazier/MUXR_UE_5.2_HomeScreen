// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "StypeEstimationByAgreement.h"

/**
* Estimates of a rate based on the wraparound to zero of consecutive values.
* For example, if a value goes from 23 to 0 MinAgree number of times, then the 
* rate will be estimated to be 24.
*/
template<typename T>
class FStypeInferredRateFromValueOverflow
{
	/** Last value given to Update function, useful to detect overflows */
    T LastValue;

	/** Rate estimator by consecutive repetitions */
    FStypeEstimationByAgreement<T> Rate;

	/** Cached flag indicated if the last Update overflowed */
	bool bOverflowed;

public:

	/**
	 * Constructor
	 *
	 * @param MinAgree     The minimum number of consecutive Rate calculations 
	 * @param DefaultRate  Default value for the rate, before an estimation based on updates is reached.
	 */
	FStypeInferredRateFromValueOverflow(uint32 MinAgree, T DefaultRate)
		: Rate(MinAgree, DefaultRate)
	{
		Reset();
	}

	/**
	 * Updates the inferred with the incoming value.
	 *
	 * @param NewValue     The varying value that eventually wraps around to zero, defining a rate.
	 */
    void Update(T NewValue)
    {
		bOverflowed = false;
		
        if((NewValue == 0) && (LastValue != 0)) // rejects repeated zeroes as overflows.
		{
			Rate.Update(LastValue+1);
			bOverflowed = true;
		}
		
		LastValue = NewValue;
    }
	
	/**
	 * Current estimation getter.
	 *
	 * @return             Current rate estimation. 
	 */
	T GetEstimation() const
	{
		return Rate.GetEstimation();
	}
	
	/** Indicates if an agreement has been reached */
	bool IsValid() const
	{
		return Rate.IsValid();
	}
	
	/** Resets current agreement. Equivalent to starting the estimation from scratch */
	void Reset()
	{
		LastValue = 0;
		bOverflowed = false;
		Rate.Reset();
	}
	
	/** Returns true only if the last value given to Update wrapped around to zero. */
	bool Overflowed() const
	{
		return bOverflowed;
	}
	
	/** 
	 * Setter of the set of acceptable rate values.	 
	 * Measured rates that are not in this set will be rejected, except when empty.
	 */
	void SetValidValues(const TSet<T>& ValidValues)
	{
		Rate.SetValidValues(ValidValues);
	}
};
