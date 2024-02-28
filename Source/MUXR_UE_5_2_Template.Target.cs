// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class MUXR_UE_5_2_TemplateTarget : TargetRules
{
	public MUXR_UE_5_2_TemplateTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "MUXR_UE_5_2_Template" } );
	}
}
