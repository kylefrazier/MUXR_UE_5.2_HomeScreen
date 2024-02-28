// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class MUXR_UE_5_2_TemplateEditorTarget : TargetRules
{
	public MUXR_UE_5_2_TemplateEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "MUXR_UE_5_2_Template" } );
	}
}
