#pragma once
#include "CoreMinimal.h"
#include "Phantom.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"
#include "GameFramework/Actor.h"
#include "Pacman/Public/PacmanGrid.h"
#include "PacmanActor.generated.h"

class USphereComponent;

UCLASS()
class PACMAN_API APacmanActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	APacmanActor();

protected:
	// Chiamato quando il gioco inizia
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* PlayerMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComp = nullptr;

	// Velocità base 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pacman")
	float Speed = 500.0f;

	// Aumento della velocità
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pacman")
	float SpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pacman")
	float ThresholdRadius = 40.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	EGridPositions LeftTeleport = EGridPositions::LeftTeleport;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	EGridPositions RightTeleport = EGridPositions::RightTeleport;

public:	
	virtual void Tick(float DeltaTime) override;

	void SetMovement(EDirection NewDirection);
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void SetGrid(AGrid* NewGrid) { Grid = NewGrid; }

	// Le seguenti funzioni e properties mi servono quando Pacman mangia il pallino speciale
	UFUNCTION()
	void OnSpeedBoostTimerExpired();

	UPROPERTY(EditAnywhere)
	float SpeedBoostDuration = 3.5f;

	FTimerHandle SpeedBoostTimerHandle;

	// Funzioni e properties per quando pacman mangia un fantasma
	void OnDeadTimerExpired();

	UPROPERTY(EditAnywhere)
	float DeadDuration = 1.5f;

	FTimerHandle DeadTimerHandle;

	bool PacmanIsInvincible = false;
	bool IsPacmanInvincible();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EState PhantomStateBeforeFrightened;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EState PhantomStateBeforeDead;

private:

	FVector MovementDirection = FVector(1.0f, 0.0f, 0.0f);

	bool bStopMovement = false;

	EDirection CurrentDirection = EDirection::Right;

	AGrid* Grid = nullptr;

	FVector Destination = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	USoundWave* eat_ghost;
};
