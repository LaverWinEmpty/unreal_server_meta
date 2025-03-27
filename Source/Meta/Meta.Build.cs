// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class Meta : ModuleRules
{
	public Meta(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		/********************************************************************************************************************
		 * Module
		 ********************************************************************************************************************/
		string[] Headers = new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
		};
		string[] Sources = new string[] {
			"OnlineSubsystemUtils"
		};

        /********************************************************************************************************************
		 * ThirdParty Paths
		 ********************************************************************************************************************/
        string ThirdPartyPath = Path.Combine(ModuleDirectory, "ThirdParty");
        string MySqlPath      = Path.Combine(ThirdPartyPath, "MySqlConnector");

        /********************************************************************************************************************
		 * ThirdParty
		 ********************************************************************************************************************/
        string[] Includes = new string[] {
            "Meta",                             // Root
			Path.Combine(MySqlPath, "include"), // SQL
        };
		string[] Libraries = new string[] {
			Path.Combine(MySqlPath, "lib64/vs14/mysqlcppconn.lib"), // SQL
		};
		string[] Plugins = new string[] {
			Path.Combine(MySqlPath, "lib64/mysqlcppconn-10-vs14.dll"), // SQL
		};
		//string[] Stagings = new string[] {
		//	"$(ProjectDir)/Binaries/Win64/mysqlcppconn.dll", // SQL
		//};

        /********************************************************************************************************************
		 * Add
		 ********************************************************************************************************************/
        PublicDependencyModuleNames.AddRange(Headers);  // add public modules
		PrivateDependencyModuleNames.AddRange(Sources); // add private modules
        PublicIncludePaths.AddRange(Includes);          // add include paths
        PublicAdditionalLibraries.AddRange(Libraries);  // add libraries path
        PublicDelayLoadDLLs.AddRange(Plugins);          // add dll fiels

		/********************************************************************************************************************
		 * Packaging
		 ********************************************************************************************************************/
		foreach(string Path in Plugins) {
			RuntimeDependencies.Add(Path);
		}
    }
}
