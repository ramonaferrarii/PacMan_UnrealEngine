#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PacmanGameModeBase.generated.h"

UCLASS()
class PACMAN_API APacmanGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


public:

	APacmanGameModeBase();

	// chiamato quando un pallino è mangiato e se è speciale avremo fantasmini blu
	void PointEat(bool IsSpecial);

public:

	// punteggio totale
	UPROPERTY(BlueprintReadOnly, Category = "Score")
		float TotalScore = 0.0f;

	//vite a disposizione al momento
	UPROPERTY(BlueprintReadOnly, Category = "Score")
		int Lives = 3;

	// vite iniziali
	UPROPERTY(EditAnywhere, Category = "Score")
		int InitialLives = 4;


	
	// punteggio pallino normale
	UPROPERTY(EditAnywhere, Category = "Score")
		float NormalPointsValue = 10.0f;

	// punteggio pallino speciale
	UPROPERTY(EditAnywhere, Category = "Score")
		float SpecialPointsValue = 50.0f;

	// secondi in cui i fantasmini diventano blu dopo aver mangiato un pallino speciale
	UPROPERTY(EditAnywhere, Category = "Score")
		float EatTimeSeconds = 3.5f;

};