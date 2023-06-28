#include "PacmanGameModeBase.h"

APacmanGameModeBase::APacmanGameModeBase()
{
	Lives = InitialLives;
}

// punteggio
void APacmanGameModeBase::PointEat(bool IsSpecial)
{
	if (IsSpecial)
	{
		TotalScore += SpecialPointsValue;

	}
	else
	{
		TotalScore += NormalPointsValue;
	}

}

