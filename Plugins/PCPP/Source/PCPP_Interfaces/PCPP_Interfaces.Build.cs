using UnrealBuildTool;

public class PCPP_Interfaces : ModuleRules {
    public PCPP_Interfaces(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Engine dependencies
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Json",
            "JsonUtilities"
        });

        // Custom dependencies
        PrivateDependencyModuleNames.AddRange(new string[] {

        });
    }
}
