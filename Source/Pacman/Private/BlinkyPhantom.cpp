#include "BlinkyPhantom.h"


void ABlinkyPhantom::BeginPlay()
{
	Super::BeginPlay();
	ChangeScatterPosition(EGridPositions::TopRight);
}
