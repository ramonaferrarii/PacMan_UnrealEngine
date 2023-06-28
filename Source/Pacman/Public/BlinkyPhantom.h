#pragma once
#include "CoreMinimal.h"
#include "Phantom.h"
#include "BlinkyPhantom.generated.h"

// Fantasmino Blinkye rosso

UCLASS()
class PACMAN_API ABlinkyPhantom : public APhantom
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
};
