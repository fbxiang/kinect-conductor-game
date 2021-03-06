// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MidiAssetEditor : ModuleRules
{
#if WITH_FORWARDED_MODULE_RULES_CTOR
	public MidiAssetEditor(ReadOnlyTargetRules Target) : base(Target)
#else
    public MidiAssetEditor(TargetInfo Target)
#endif
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				"AssetTools",
				"MainFrame",
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				"MidiAssetEditor/Private",
				"MidiAssetEditor/Private/Factories",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"ContentBrowser",
				"Core",
				"CoreUObject",
                "DesktopWidgets",
				"EditorStyle",
				"Engine",
				"InputCore",
                "Projects",
				"Slate",
				"SlateCore",
                "MidiAsset",
				"UnrealEd",
			}
		);

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
				"AssetTools",
				"UnrealEd",
			}
		);
	}
}
