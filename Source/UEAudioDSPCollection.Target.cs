using UnrealBuildTool;
using System.Collections.Generic;

public class UEAudioDSPCollectionTarget : TargetRules
{
	public UEAudioDSPCollectionTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion	 = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "UEAudioDSPCollection" } );
	}
}
