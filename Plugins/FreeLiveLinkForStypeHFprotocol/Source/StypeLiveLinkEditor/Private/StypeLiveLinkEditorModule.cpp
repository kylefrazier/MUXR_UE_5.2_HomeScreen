// Copyright Epic Games, Inc. All Rights Reserved.

#include "StypeLiveLinkEditorModule.h"

#include "StypeLiveLinkFactory.h"
#include "StypeLiveLinkSourcePanel.h"

class FStypeLiveLinkEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
		// register the LiveLinkFactory panel
		auto BuildCreationPanel = [](const UStypeLiveLinkSourceFactory* Factory, ULiveLinkSourceFactory::FOnLiveLinkSourceCreated OnSourceCreated)
			-> TSharedPtr<SWidget>
		{
			return SNew(SStypeLiveLinkSourcePanel)
				.Factory(Factory)
				.OnSourceCreated(OnSourceCreated);
		};
		UStypeLiveLinkSourceFactory::OnBuildCreationPanel.BindLambda(BuildCreationPanel);
	}

	virtual void ShutdownModule() override
	{
		UStypeLiveLinkSourceFactory::OnBuildCreationPanel.Unbind();
	}
};


IMPLEMENT_MODULE(FStypeLiveLinkEditorModule, StypeLiveLinkEditor)
