// Copyright Epic Games, Inc. All Rights Reserved.

#include "StypeLiveLinkFactory.h"

#include "ILiveLinkClient.h"
#include "StypeLiveLinkSource.h"

#define LOCTEXT_NAMESPACE "StypeLiveLinkSourceFactory"


UStypeLiveLinkSourceFactory::FBuildCreationPanelDelegate UStypeLiveLinkSourceFactory::OnBuildCreationPanel;

FText UStypeLiveLinkSourceFactory::GetSourceDisplayName() const
{
	return LOCTEXT("SourceDisplayName", "Stype");
}

FText UStypeLiveLinkSourceFactory::GetSourceTooltip() const
{
	return LOCTEXT("SourceTooltip", "Creates a connection to a Stype UDP stream");
}

TSharedPtr<SWidget> UStypeLiveLinkSourceFactory::BuildCreationPanel(FOnLiveLinkSourceCreated InOnLiveLinkSourceCreated) const
{
	if (OnBuildCreationPanel.IsBound())
	{
		return OnBuildCreationPanel.Execute(this, InOnLiveLinkSourceCreated);
	}
	return TSharedPtr<SWidget>();
}

TSharedPtr<ILiveLinkSource> UStypeLiveLinkSourceFactory::CreateSource(const FString& ConnectionString) const
{
	FStypeLiveLinkConnectionSettings Settings;
	if (!ConnectionString.IsEmpty())
	{
		FStypeLiveLinkConnectionSettings::StaticStruct()->ImportText(*ConnectionString, &Settings, nullptr, EPropertyPortFlags::PPF_None, nullptr, FStypeLiveLinkConnectionSettings::StaticStruct()->GetName(), true);
	}
	return MakeShared<FStypeLiveLinkSource>(Settings);
}

FString UStypeLiveLinkSourceFactory::CreateConnectionString(const FStypeLiveLinkConnectionSettings& Settings)
{
	FString ConnectionString;
	FStypeLiveLinkConnectionSettings::StaticStruct()->ExportText(ConnectionString, &Settings, nullptr, nullptr, EPropertyPortFlags::PPF_None, nullptr, true);
	return ConnectionString;
}

#undef LOCTEXT_NAMESPACE