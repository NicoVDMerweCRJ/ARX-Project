// Copyright 2018-2019 David Romanski (Socke). All Rights Reserved.

using UnrealBuildTool;

public class HTMLMenuMain : ModuleRules
{
	public HTMLMenuMain(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "WebBrowser",
                    "Slate",
                    "SlateCore",
                    "UMG",
                    "Engine",
                    "Networking",
                    "Sockets"
                    
                }
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "WebBrowser",
                "Slate",
                "SlateCore",
                "UMG",
                "Engine",
                "Networking",
                "Sockets"
            }
            );

        if (Target.bBuildEditor == true)
        {
            //@TODO: UnrealEd Needed for the triangulation code used for sprites (but only in editor mode)
            //@TOOD: Try to move the code dependent on the triangulation code to the editor-only module
            PrivateIncludePathModuleNames.AddRange(
                new string[]
                {
                        "UnrealEd",
                }
            );
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                        "UnrealEd",
                }
            );
        }

        if (Target.bBuildEditor || Target.Platform == UnrealTargetPlatform.Android || Target.Platform == UnrealTargetPlatform.IOS)
        {
            // WebBrowserTexture required for cooking on Android and IOS
            PrivateIncludePathModuleNames.AddRange(
                new string[]
                {
                        "WebBrowserTexture",
                }
            );
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                        "WebBrowserTexture",
                }
            );
        }
    }
}
