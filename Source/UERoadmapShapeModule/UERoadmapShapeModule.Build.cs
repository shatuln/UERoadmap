using UnrealBuildTool;

public class UERoadmapShapeModule : ModuleRules
{
    public UERoadmapShapeModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "UnrealEd",
            "LevelEditor",
            "Slate",
            "SlateCore",
            "DynamicMesh",
            "ToolMenus",
            "GeometryFramework",
            "GeometryCore"
        });
    }
}