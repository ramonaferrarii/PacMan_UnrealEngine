#pragma once
#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"
#include "GameFramework/Actor.h"
#include "PacmanGrid.h"
#include "Phantom.generated.h"

class AGrid;
class USphereComponent;

// Definizione degli stati 
UENUM(BlueprintType)
enum class EState : uint8
{
	Chase,
	Scatter,
	Frightened,
	Dead
};

UCLASS()
class PACMAN_API APhantom : public AActor
{
	GENERATED_BODY()
	
public:	
	APhantom();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* PhantomMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComp = nullptr;

	//Metodi e membri protetti della classe APhantom. 
	// Il metodo BeginPlay() viene chiamato quando il gioco inizia.
	//Vengono definiti membri come la velocità base, il moltiplicatore di velocità,
	// la durata del movimento in modalità "Scatter" e "Chase", l'impugnatura del timer per la modalità "Scatter",
	// le posizioni di destinazione per le modalità "Scatter", "Frightened" e "Dead", le posizioni di teletrasporto.

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pacman")
	float Speed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pacman")
	float SpeedMultiplier = 1.0f;

	UFUNCTION()
	void OnScatterTimerExpired();

	UPROPERTY(EditAnywhere)
	float ScatterDuration = 7.0f;

	UPROPERTY(EditAnywhere)
	float ChaseDuration = 20.0f;

	FTimerHandle ScatterTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	EGridPositions ScatterPosition = EGridPositions::TopLeft;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	EGridPositions GhostsHouse = EGridPositions::Middle;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	EGridPositions LeftTeleport = EGridPositions::LeftTeleport;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	EGridPositions RightTeleport = EGridPositions::RightTeleport;


public:	
	virtual void Tick(float DeltaTime) override;

	void SetGrid(AGrid* NewGrid);

	void SetStartDirection(FVector StartDirection) { MovementDir = StartDirection; }

	void ChangeState(EState NewState);

	void SetSpeedMultiplier(float Value);

	EState GetState();

	void ChangeScatterPosition(EGridPositions NewPosition);

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void ChangeDirection();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EState CurrentState = EState::Scatter;

	virtual FVector GetTargetPosition();

	FVector MovementDir = FVector(-1.0f, 0.0f, 0.0f);

	FVector NextMovementDir = FVector(-1.0f, 0.0f, 0.0f);

	FVector Destination = FVector::ZeroVector;

	AGrid* Grid = nullptr;

	int XTileDestination = 0;
	int YTileDestination = 0;

private:
	UPROPERTY(EditAnywhere)
	USoundWave* minuslife;

};
