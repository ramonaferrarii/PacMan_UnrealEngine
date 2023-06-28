using UnrealBuildTool;
using System.Collections.Generic;

public class PacmanTarget : TargetRules
{
	public PacmanTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "Pacman" } );
	}
}
