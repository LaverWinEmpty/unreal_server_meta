// Copyright Epic Games, Inc. All Rights Reserved.

using System.Collections.Generic;
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
            "PlatformCryptoOpenSSL",
            "OpenSSL",
        };
		string[] Sources = new string[] {
			//
        };

        /********************************************************************************************************************
		 * ThirdParty Paths
		 ********************************************************************************************************************/
        string ThirdPartyPath = Path.Combine(ModuleDirectory, "../../ThirdParty");
        string MySqlPath      = Path.Combine(ThirdPartyPath, "MySqlConnector");

        /********************************************************************************************************************
		 * ThirdParty
		 ********************************************************************************************************************/
        string[] Includes = new string[] {
            "Meta",								// Root
			Path.Combine(MySqlPath, "include"),	// MySQL X Protocol Header
        };
		string[] Libraries = new string[] {
			// Path.Combine(MySqlPath, "lib64/vs14/mysqlcppconnx-static.lib"),	// MySQL X Protocol lib
			// Path.Combine(MySqlPath, "lib64/vs14/mysqlcppconnx.lib"),			// MySQL X Protocol dll
			Path.Combine(MySqlPath, "lib64/vs14/mysqlcppconn-static.lib"),	// MySQL lib
			Path.Combine(MySqlPath, "lib64/vs14/mysqlcppconn.lib"),			// MySQL dll
			Path.Combine(MySqlPath, "lib64/vs14/libssl.lib"),				// MySQL OpenSSL
			Path.Combine(MySqlPath, "lib64/vs14/libcrypto.lib"),			// MySQL OpenSSL
		};
		Dictionary<string, string> Plugins = new Dictionary<string, string>{
			// { "mysqlcppconnx-2-vs14.dll",	Path.Combine(MySqlPath, "lib64") },	// MySQL X Protocol
			{ "mysqlcppconn-10-vs14.dll",	Path.Combine(MySqlPath, "lib64") },	// MySQL
			{ "libssl-3-x64.dll",			Path.Combine(MySqlPath, "lib64") },	// MySQL OpenSSL
			{ "libcrypto-3-x64.dll",		Path.Combine(MySqlPath, "lib64") },	// MySQL OpenSSL
        };

        /********************************************************************************************************************
		 * Add
		 ********************************************************************************************************************/
        PublicDependencyModuleNames.AddRange(Headers);  // add public modules
		PrivateDependencyModuleNames.AddRange(Sources); // add private modules
        PublicIncludePaths.AddRange(Includes);          // add include paths
        PublicAdditionalLibraries.AddRange(Libraries);  // add libraries path
        foreach (var Plugin in Plugins) {
            RuntimeDependencies.Add(
                Path.Combine("$(BinaryOutputDir)", Plugin.Key), // cloned dll
                Path.Combine(Plugin.Value, Plugin.Key)          // source dll
            );
        }
		// end
    }
}
