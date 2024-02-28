// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StypeLiveLinkEditor : ModuleRules
{
	public StypeLiveLinkEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"StypeLiveLink",
			});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"PropertyEditor",
				"Slate",
				"SlateCore",
			});
	}
}
