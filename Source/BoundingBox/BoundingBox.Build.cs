// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BoundingBox : ModuleRules
{
	public BoundingBox(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core",
															"CoreUObject",
															"Engine",
															"RHI",
															"Renderer",
															"RenderCore",
															"Projects",
                                                            "GeometryCache"
                                                        });


        PrivateDependencyModuleNames.AddRange(new string[] {  });

	}
}
