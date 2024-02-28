// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Set.h"

/**
* Estimation of a value of type T based on a required number of consecutive agreements.
*/

template<typename T>
class FStypeEstimationByAgreement
{
	/** Required number of agreements before updating an estimation */
	const uint32 MinAgree;
	
	/** Default estimation. Useful when an agreement has not been reached yet. */
	const T DefaultEstimation;

	/** Last value given to Update */
	T LastValue;                
	
	/** Current estimation */
	T Estimation;

	/** Current number of agreements. */
	uint32 NumAgree;
	
	/** Keeps track of whether an agreement has been reached. */
	bool bIsValid;

	/** Optional set of valid values to agree on.  */
	TSet<T> ValidValues;

public:

	/**
	 * Constructor
	 *
	 * @param MinAgree     The minimum number of consecutive Rate calculations
	 * @param DefaultRate  Default value for the rate, before an estimation based on updates is reached.
	 */
	FStypeEstimationByAgreement(uint32 MinAgree, T DefaultEstimation)
		: MinAgree(MinAgree)
		, DefaultEstimation(DefaultEstimation)
	{
		Reset();
	}

	/** Resets current agreement. Equivalent to starting the estimation from scratch */
	void Reset()
	{
		LastValue = 0;
		Estimation = DefaultEstimation;
		NumAgree = 0;
		bIsValid = false;

		// We don't reset set of ValidValues.
	}

	/** Update the estimation with a new proposed value */
	void Update(T NewValue)
	{
		if (NewValue == LastValue)
		{
			if (++NumAgree >=  MinAgree)
			{
				NumAgree = MinAgree;

				// reject invalid values
				if (!ValidValues.Num() || ValidValues.Find(NewValue))
				{
					Estimation = NewValue;
					bIsValid = true;
				}
			}
		}
		else
		{
			NumAgree = 0;
		}

		LastValue = NewValue;
	}

	/** Returns the current estimation. */
	T GetEstimation() const
	{
		return Estimation;
	}

	/** Indicates if an agreement has been reached */
	bool IsValid() const
	{
		return bIsValid;
	}

	/** Sets possible valid values. If not empty, any new potential agreement is rejected if it does not exist in this set */
	void SetValidValues(const TSet<T>& NewValidValues)
	{
		ValidValues = NewValidValues;
	}
};
