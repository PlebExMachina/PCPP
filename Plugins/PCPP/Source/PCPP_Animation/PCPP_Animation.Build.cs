using UnrealBuildTool;

public class PCPP_Animation : ModuleRules {
    public PCPP_Animation(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Engine dependencies
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
			"PCPP_Components",
            "Dev"
        });

        // Custom dependencies
        PrivateDependencyModuleNames.AddRange(new string[] {
            // TODO: Add custom (i.e. third party / not UE) dependencies here
        });
    }
}
