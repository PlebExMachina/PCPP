using UnrealBuildTool;

public class PCPP_AI : ModuleRules {
    public PCPP_AI(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Engine dependencies
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "AIModule",
            "GameplayTasks",
        });

        // Custom dependencies
        PrivateDependencyModuleNames.AddRange(new string[] {
            // TODO: Add custom (i.e. third party / not UE) dependencies here
        });
    }
}
