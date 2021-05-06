using UnrealBuildTool;

public class Dev : ModuleRules {
    public Dev(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Engine dependencies
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
        });

        // Custom dependencies
        PrivateDependencyModuleNames.AddRange(new string[] {
            // TODO: Add custom (i.e. third party / not UE) dependencies here
        });
    }
}
