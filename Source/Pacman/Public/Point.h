#pragma once
#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"
#include "GameFramework/Actor.h"
#include "Point.generated.h"

class USoundWave;

//Dichiarazione della classe APoint che eredita dalla classe AActor. 
//Questa classe rappresenta un punto nel gioco Pacman.

UCLASS()
class PACMAN_API APoint : public AActor
{
	GENERATED_BODY()

	//Costruttore predefinito della classe APoint.
public:	
	APoint();

	//Metodo Eat() che rappresenta l'azione di "mangiare" il punto.
	void Eat();

	//Metodo SetSpecial() che imposta se il punto è speciale o no.
	void SetSpecial(bool bSpecial);

	//Metodo GetSpecial() che restituisce se il punto è speciale o no.
	bool GetSpecial();

protected:
	// chiamato quando il gioco comincia
	virtual void BeginPlay() override;

	//Puntatore a un componente di mesh statica utilizzato per rappresentare 
	//il punto nel gioco. 
	UPROPERTY(VisibleAnywhere, Category = "Pacman")
	UStaticMeshComponent* PointMesh = nullptr;

	//Flag che indica se il punto è speciale o no. 
	UPROPERTY(EditAnywhere, Category = "Pacman")
	bool bIsSpecial = false;

	//La grandezza del punto speciale.
	UPROPERTY(EditAnywhere, Category = "Pacman")
	float SpecialScale= 0.6;

	// suono associato al punto mangiato
private:
	UPROPERTY(EditAnywhere)
	USoundWave* pacman_food;

};
