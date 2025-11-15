using UnrealBuildTool;
using System.Collections.Generic;

public class UEAudioDSPCollectionTarget : TargetRules
{
	public UEAudioDSPCollectionTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion	 = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "UEAudioDSPCollection" } );
	}
}
