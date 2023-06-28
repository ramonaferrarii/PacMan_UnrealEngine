#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PacmanGrid.generated.h"

class AStaticMeshActor;
class APoint;
class APacmanActor;
class APhantom;

// EDirection che rappresenta le possibili direzioni di movimento.
enum class EDirection : uint8
{
	Up,
	Down,
	Left,
	Right,
};

// EGridPositions rappresenta le diverse posizioni nella griglia di gioco.

UENUM(BlueprintType)
enum class EGridPositions : uint8
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	PlayerPosition,
	Middle,
	LeftTeleport,
	RightTeleport
};

 //  ESearchOrder che specifica l'ordine di ricerca per le prossime destinazioni, basato sulla distanza minima o casuale.
enum class ESearchOrder : uint8
{
	MinDistance,
	Random
};

// FTileSelection rappresenta una selezione di tile nella griglia di gioco.
// Contiene le coordinate X e Y del tile, oltre alla direzione associata alla tile.
USTRUCT()
struct FTileSelection
{
	GENERATED_USTRUCT_BODY()
	int XTile = 0;
	int YTile = 0;
	FVector TileDirection = FVector(0.0, -1.0f, 0.0f);

	FTileSelection()
	{

	}

	FTileSelection(int NewX, int NewY, FVector NewDir)
	{
		XTile = NewX;
		YTile = NewY;
		TileDirection = NewDir;
	}


	int GetManhattanDistance(int XTargetTile, int YTargetTile)
	{
		return  FMath::Abs(XTargetTile - XTile) + FMath::Abs(YTargetTile - YTile);
	}
};

// Classe AGrid rappresenta la griglia di gioco

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACMAN_API AGrid : public AActor
{
	GENERATED_BODY()

public:	
	// setto dei valori di default per le proprietà 
	AGrid();

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	TSubclassOf<AStaticMeshActor> WallClass;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	TSubclassOf<APoint> PointClass;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	TSubclassOf<APacmanActor> PacmanActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	TArray<TSubclassOf<APhantom>> PhantomClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	int GridScale = 100;

	UFUNCTION(BlueprintCallable, Category = "Pacman")
	APacmanActor* GetPacman() { return PacmanActor; }

	// mi ritorna vero o false in base al fatto se il movimento può essere fatto meno
	bool CanPlayerMoveInDir(EDirection NewDirection);

	bool CanMoveToTile(const int& XTile, const int& YTile);

	// mi dice se un giocatore può muoversi seguendo la nuova direzione (NewDirection)
	bool CalculateNewDestination(EDirection NewDirection, FVector& Destination);


	FVector GetGridSpecialPosition(EGridPositions GridChoice);

	FVector GetNextDestination(const FVector& PositionToGo, const FVector& CurrentPosition, const FVector& CurrentDirection, FVector& NextDirection, ESearchOrder SearchOrder = ESearchOrder::MinDistance);

	FVector GetVectorDirection(const EDirection& NewDirection);

protected:
	// Chiamato quando inizia il gioco
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	APacmanActor* PacmanActor = nullptr;

	//conversione
	float Converser = 1.0f;

	void GetTileFromWorld(const FVector& WorldPosition, int& XTile, int& YTile);

	char GetTileValue(int& XTile, int& YTile);

private:
	
	FVector GetWorldFromTile(const int& XTile, const int& YTile);

	void GetTileInDir(EDirection NextDirection, int& XTile, int& YTile);

	TArray<FTileSelection>GetPossibleTiles(int XTile, int YTile, EDirection CurrentDir);

	EDirection GetDirection(const FVector& Dir);

	TArray<APhantom*> Phantoms;
};
