using UnrealBuildTool;

public class PCPP_Components : ModuleRules {
    public PCPP_Components(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Engine dependencies
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Dev",
            "PCPP_INTERFACES",
            "Json",
            "JsonUtilities"
        });

        // Custom dependencies
        PrivateDependencyModuleNames.AddRange(new string[] {
			
        });
    }
}
