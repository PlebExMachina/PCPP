using UnrealBuildTool;

public class PCPP_Components : ModuleRules {
    public PCPP_Components(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Engine dependencies
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Dev"
        });

        // Custom dependencies
        PrivateDependencyModuleNames.AddRange(new string[] {
			
        });
    }
}
