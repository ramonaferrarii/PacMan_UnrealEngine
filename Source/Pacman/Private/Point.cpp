#include "Point.h"
#include "PacmanGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"

// Costruttore della classe APoint che imposta il flag bCanEverTick su false. 
// Crea un componente UStaticMeshComponent denominato "PointMesh" come membro predefinito dell'oggetto.
APoint::APoint()
{
	PrimaryActorTick.bCanEverTick = false;
	PointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PointMesh"));
}


//Metodo Eat() che viene chiamato quando il punto viene "mangiato".
// + il suono associato al punto(pacman_food)

void APoint::Eat()
{
	if (APacmanGameModeBase* GameMode = Cast<APacmanGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		UGameplayStatics::PlaySound2D(this, pacman_food);
		GameMode->PointEat(bIsSpecial);
		Destroy();
	}
}

//Metodo SetSpecial() che imposta la  flag bIsSpecial in base al valore fornito come parametro.
//Se il punto è speciale, viene impostata la scala relativa dell'attore utilizzando il valore SpecialScale.

void APoint::SetSpecial(bool bSpecial)
{
	bIsSpecial = bSpecial;
	if (bIsSpecial)
	{
		SetActorRelativeScale3D(FVector(SpecialScale));
	}
}

// Metodo GetSpecial() che restituisce il valore corrente del flag bIsSpecial.
bool APoint::GetSpecial()
{
	return bIsSpecial;
}

// Chiamato quando il gioco comincia
void APoint::BeginPlay()
{
	Super::BeginPlay();
	SetSpecial(bIsSpecial);
}
