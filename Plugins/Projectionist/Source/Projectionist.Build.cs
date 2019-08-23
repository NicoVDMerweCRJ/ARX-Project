// Copyright 2019 blurryroots interactive, Sven Freiberg. All Rights Reserved.

using UnrealBuildTool;

public class Projectionist : ModuleRules
{
	public Projectionist(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "MediaAssets"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {
        });
	}
}
