// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

using UnrealBuildTool;
using System.IO;

public class FileSystemLibrary : ModuleRules
{
	public FileSystemLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
			}
		);

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicDefinitions.Add("WINDOWS_IGNORE_PACKING_MISMATCH");
		}

		if (Target.Platform == UnrealTargetPlatform.Android)
		{			
			string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "FileSystemLibrary_UPL.xml"));
			
			PrivateDependencyModuleNames.AddRange(new string[] { "Launch" });
		}
	}
}
