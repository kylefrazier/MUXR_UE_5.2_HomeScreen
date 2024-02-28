// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "LiveLinkSourceFactory.h"
#include "StypeLiveLinkSource.h"
#include "StypeLiveLinkFactory.generated.h"


class SStypeLiveLinkSourcePanel;

UCLASS()
class STYPELIVELINK_API UStypeLiveLinkSourceFactory : public ULiveLinkSourceFactory
{
public:
	DECLARE_DELEGATE_RetVal_TwoParams(TSharedPtr<SWidget>, FBuildCreationPanelDelegate, const UStypeLiveLinkSourceFactory*, FOnLiveLinkSourceCreated);
	static FBuildCreationPanelDelegate OnBuildCreationPanel;

public:
	GENERATED_BODY()

	virtual FText GetSourceDisplayName() const;
	virtual FText GetSourceTooltip() const;

	virtual EMenuType GetMenuType() const override { return EMenuType::SubPanel; }
	virtual TSharedPtr<SWidget> BuildCreationPanel(FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const override;
	virtual TSharedPtr<ILiveLinkSource> CreateSource(const FString& ConnectionString) const override;

	static FString CreateConnectionString(const FStypeLiveLinkConnectionSettings& Settings);
};
