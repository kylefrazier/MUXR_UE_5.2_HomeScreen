// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StypeLiveLink : ModuleRules
{
	public StypeLiveLink(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"LiveLinkInterface",
                "Networking",
                "Sockets",
                "LevelSequence",
                "AssetRegistry",
                "MovieScene",
                "MovieSceneTracks",
                "MediaFrameworkUtilities",
                "MediaIOCore", 
                "LiveLinkLens",
                "CameraCalibrationCore",
                "MovieSceneCapture"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "InputCore", 
                "LiveLinkCamera",
                "LiveLinkComponents",
                "CameraCalibrationCore"
			}
			);
		
		if (Target.bBuildEditor == true)
		{
			PrivateDependencyModuleNames.Add("CameraCalibrationEditor");
		}

	}
}
