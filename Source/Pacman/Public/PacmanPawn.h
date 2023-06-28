#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PacmanPawn.generated.h"

class APacmanActor;

UCLASS()
class PACMAN_API APacmanPawn : public APawn
{
	GENERATED_BODY()

public:
	APacmanPawn();

	// Chiamato quando inizia il gioco
protected:

	void BeginPlay() override;

	//Metodo pubblico virtuale utilizzato per configurare i componenti di input del giocatore.
public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//Metodi privati UpInput(), DownInput(), RightInput(), LeftInput() che gestiscono gli input dell'utente
	// per il movimento
private:

	void UpInput();
	void DownInput();
	void RightInput();
	void LeftInput();

	void SearchForPlayer();
	
	APacmanActor* PacmanActor = nullptr;
};
