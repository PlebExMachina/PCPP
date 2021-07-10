using UnrealBuildTool;

public class PCPP_UI : ModuleRules {
    public PCPP_UI(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Engine dependencies
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "UMG",
            "PCPP_Components",
            "PCPP_Interfaces"
        });

        // Custom dependencies
        PrivateDependencyModuleNames.AddRange(new string[] {
            // TODO: Add custom (i.e. third party / not UE) dependencies here
        });
    }
}
