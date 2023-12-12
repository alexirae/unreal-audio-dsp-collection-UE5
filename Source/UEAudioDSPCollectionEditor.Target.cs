using UnrealBuildTool;
using System.Collections.Generic;

public class UEAudioDSPCollectionEditorTarget : TargetRules
{
	public UEAudioDSPCollectionEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion	 = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "UEAudioDSPCollection" } );
	}
}
