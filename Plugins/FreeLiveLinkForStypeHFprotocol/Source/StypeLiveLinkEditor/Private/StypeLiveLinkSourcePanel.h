// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "StypeLiveLinkFactory.h"
#include "StypeLiveLinkSource.h"

class SStypeLiveLinkSourcePanel : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SStypeLiveLinkSourcePanel) {}
		SLATE_ARGUMENT(const UStypeLiveLinkSourceFactory*, Factory)
		SLATE_EVENT(ULiveLinkSourceFactory::FOnLiveLinkSourceCreated, OnSourceCreated)
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);

public:
	FReply CreateNewSource(bool bShouldCreateSource);

private:
	FStypeLiveLinkConnectionSettings ConnectionSettings;
	TWeakObjectPtr<const UStypeLiveLinkSourceFactory> SourceFactory;
	ULiveLinkSourceFactory::FOnLiveLinkSourceCreated OnSourceCreated;
};