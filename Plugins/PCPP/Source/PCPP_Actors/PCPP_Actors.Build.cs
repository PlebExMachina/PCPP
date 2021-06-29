using UnrealBuildTool;

public class PCPP_Actors : ModuleRules {
    public PCPP_Actors(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Engine dependencies
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "PCPP_Components",
            "PCPP_Interfaces",
            "Json",
            "JsonUtilities",
        });

        // Custom dependencies
        PrivateDependencyModuleNames.AddRange(new string[] {
            // TODO: Add custom (i.e. third party / not UE) dependencies here
        });
    }
}
