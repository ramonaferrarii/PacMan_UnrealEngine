#include "PacmanGrid.h"
#include "Engine/StaticMeshActor.h"
#include "Point.h"
#include "Phantom.h"
#include "PacmanActor.h"
#include "DrawDebugHelpers.h"


// Devo costruire la mappa, ovvero il labirinto

// Legenda:
// # - Wall
// B - Big Point
// N - Empty
// P - Player
// F - Phantom
// F - Cherry
// T - Teleport
// L - No Food

// Definisco le dimensioni della mappa attraverso due variabili costanti
const int MapSizeX = 30;
const int MapSizeY = 29;

// Definisco la mappa attraverso la legenda definita in precedenza:
const char Map[MapSizeX][MapSizeY] = {
	"############################",
	"#            ##            #",
	"# #### ##### ## ##### #### #",
	"#B#### ##### ## ##### ####B#",
	"# #### ##### ## ##### #### #",
	"#                          #",
	"# #### ## ######## ## #### #",
	"# #### ## ######## ## #### #",
	"#      ##    ##    ##      #",
	"###### #####L##L##### ######",
	"###### #####L##L##### ######",
	"###### ##LLFLLFLFLL## ######",
	"###### ##L###NN###L## ######",
	"###### ##L#NNNNNN#L## ######",
	"TLLLLL LLL#NNNNNN#LLL LLLLLT",
	"###### ##L#NNNNNN#L## ######",
	"###### ##L########L## ######",
	"###### ##LLLLFLLLLL## ######",
	"###### ##L########L## ######",
	"###### ##L########L## ######",
	"#            ##            #",
	"# #### ##### ## ##### #### #",
	"# #### ##### ## ##### #### #",
	"#B  ##       P        ##  B#",
	"### ## ## ######## ## ## ###",
	"#      ## ######## ##      #",
	"# #######    ##    ####### #",
	"# ########## ## ########## #",
	"#                          #",
	"############################",
};



// Setto i valori di default
AGrid::AGrid()
{
	// Setto l'inizializzazione di questo componenete quando il gioco comincia
	PrimaryActorTick.bCanEverTick = true;
}

// Quando il gioco inizia viene chiamato:
void AGrid::BeginPlay()
{
	Super::BeginPlay();

	//Ogni oggetto viene fatto "partire" da suo punto iniziale
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector SpawnPosition = GetActorLocation();

	FVector HalfGrid(GridScale*0.5f, GridScale*0.5, 0.0f);

	SpawnPosition += HalfGrid;
	float InitialX = SpawnPosition.X;

	Converser = 1.0f / GridScale;

	// Creazione dei nodi attraverso ciclo for
	for (int i = 0; i < MapSizeX; i++)
	{
		for (int j = 0; j < MapSizeY - 1; j++)
		{
			char MapTile = Map[i][j];
			if (MapTile == '#')
			{
				GetWorld()->SpawnActor<AStaticMeshActor>(WallClass, SpawnPosition, FRotator::ZeroRotator, ActorSpawnParams);
			}
			else if (MapTile == 'P' && !PacmanActor)
			{
				PacmanActor = GetWorld()->SpawnActor<APacmanActor>(PacmanActorClass, SpawnPosition, FRotator::ZeroRotator, ActorSpawnParams);
				PacmanActor->SetGrid(this);
			}
			else if (MapTile == 'F')
			{
				TSubclassOf<APhantom> PhantomClass = Phantoms.Num() < PhantomClasses.Num() ? PhantomClasses[Phantoms.Num()] : PhantomClasses[0];
				APhantom* Phantom = GetWorld()->SpawnActor<APhantom>(PhantomClass, SpawnPosition, FRotator::ZeroRotator, ActorSpawnParams);
				Phantom->SetGrid(this);
				Phantoms.Add(Phantom);
			}
			else if(MapTile != 'N' && MapTile != 'T' && MapTile != 'L')
			{
				APoint* Point = GetWorld()->SpawnActor<APoint>(PointClass, SpawnPosition, FRotator::ZeroRotator, ActorSpawnParams);
				Point->SetSpecial(MapTile == 'B');
			}
			SpawnPosition.X += GridScale;
		}
		SpawnPosition.X = InitialX;
		SpawnPosition.Y += GridScale;
	}
}

void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Definisco la funzione che mi restituisce tramite conversione la posizione della tile
void AGrid::GetTileFromWorld(const FVector & WorldPosition, int & XTile, int & YTile)
{
	FVector GridPosition = WorldPosition - GetActorLocation();
	XTile = GridPosition.Y * Converser;
	YTile = GridPosition.X * Converser;
}

// Funzione per avere la posizione della tile nella mappa, usata per calcolare la destinazionedi un attore
FVector AGrid::GetWorldFromTile(const int & XTile, const int & YTile)
{
	FVector HalfGrid(GridScale*0.5f, GridScale*0.5, 0.0f);
	FVector Destination = FVector::ZeroVector;
	Destination.X = YTile * GridScale;
	Destination.Y = XTile * GridScale;
	Destination += GetActorLocation();
	Destination.Z = 0;
	Destination += HalfGrid;
	return Destination;
}

char AGrid::GetTileValue(int& XTile, int& YTile)
{
	return Map[XTile][YTile];
}

// Mi restituisce la posizione della tile successiva a quella in cui sono, data una direzione.
// implementato con switch... case... case
void AGrid::GetTileInDir(EDirection NextDirection, int & XTile, int & YTile)
{
	if (XTile < MapSizeX && YTile < MapSizeY)
	{
		switch (NextDirection)
		{
		case EDirection::Up:
		{
			XTile -= 1;
			break;
		}
		case EDirection::Down:
		{
			XTile += 1;
			break;
		}
		case EDirection::Right:
		{
			YTile += 1;
			break;
		}
		case EDirection::Left:
		{
			YTile -= 1;
			break;
		}
		default:
			break;
		}
	}
}


// Devo verificare che effettivamente una tile sia raggiungibile (quindi non  sia per esempio un muro) e il personaggio ci possa andare sopra
bool AGrid::CalculateNewDestination(EDirection NewDirection, FVector& Destination)
{

	if (PacmanActor)
	{
		int XTile, YTile = 0;
		GetTileFromWorld(PacmanActor->GetActorLocation(), XTile, YTile);
		int XInitTile = XTile;
		int YInitTile = YTile;

		GetTileInDir(NewDirection, XTile, YTile);

		FVector HalfGrid(GridScale*0.5f, GridScale*0.5, 0.0f);
		if (!CanMoveToTile(XTile, YTile))
		{
			Destination = GetWorldFromTile(XInitTile, YInitTile);
			return false;
		}
		else
		{
			Destination = GetWorldFromTile(XTile, YTile);
			return true;
		}
		
		
	}
	return false;
}

// Verifico che il personaggio si possa muovere in quella direzione
bool AGrid::CanPlayerMoveInDir(EDirection NewDirection)
{
	if (PacmanActor)
	{
		int XTile, YTile = 0;
		GetTileFromWorld(PacmanActor->GetActorLocation(), XTile, YTile);
		GetTileInDir(NewDirection, XTile, YTile);
		return CanMoveToTile(XTile, YTile);
	}
	else
	{
		return false;
	}
}

bool AGrid::CanMoveToTile(const int & XTile, const int & YTile)
{
	return Map[XTile][YTile] != '#' && Map[XTile][YTile] != 'N';
}


FVector AGrid::GetGridSpecialPosition(EGridPositions GridChoice)
{
	int XTile, YTile = 0;
	switch (GridChoice)
	{
	case EGridPositions::TopLeft:
	{
		XTile = 0;
		YTile = 0;
		break;
	}
	case EGridPositions::TopRight:
	{
		XTile = 0;
		YTile = MapSizeY;
		break;
	}
	case EGridPositions::BottomLeft:
	{
		XTile = MapSizeX;
		YTile = 0;
		break;
	}
	case EGridPositions::BottomRight:
	{
		XTile = MapSizeY;
		YTile = MapSizeX;
		break;
	}
	case EGridPositions::PlayerPosition:
	{
		if (PacmanActor)
		{
			return PacmanActor->GetActorLocation();
		}
		break;
	}
	case EGridPositions::Middle:
	{
		XTile = 11;
		YTile = 13;
		break;
	}

	case EGridPositions::LeftTeleport:
	{
		XTile = 14;
		YTile = 1;
		break;
	}

	case EGridPositions::RightTeleport:
	{
		XTile = 14;
		YTile = 27;
		break;
	}

	default:
		break;
	}
	return GetWorldFromTile(XTile, YTile);
}

FVector AGrid::GetNextDestination(const FVector& PositionToGo, const FVector& CurrentPosition, const FVector& CurrentDirection, FVector& NextDirection, ESearchOrder SearchOrder)
{
	int XTile, YTile = 0;
	int XTargetTile, YTargetTile = 0;

	GetTileFromWorld(CurrentPosition, XTile, YTile);
	GetTileFromWorld(PositionToGo, XTargetTile, YTargetTile);

	TArray<FTileSelection> PossibleTiles = GetPossibleTiles(XTile, YTile, GetDirection(CurrentDirection));
	check(PossibleTiles.Num() > 0);


	int MinDistance = 1000;
	FTileSelection SelectedTile;
	if (PossibleTiles.Num() > 1)
	{
	
		switch (SearchOrder)
		{
		case ESearchOrder::MinDistance:
		{
			for (int i = 0; i < PossibleTiles.Num(); i++)
			{
				int MDistance = PossibleTiles[i].GetManhattanDistance(XTargetTile, YTargetTile);
				if (MDistance < MinDistance)
				{
					MinDistance = MDistance;
					SelectedTile = PossibleTiles[i];
				}
			}
			break;
		}
		case ESearchOrder::Random:
		{
			int RandomIndex = FMath::RandRange(0, PossibleTiles.Num() - 1);
			SelectedTile = PossibleTiles[RandomIndex];
		}
		default:
		{
			// Di defautl imposto la minima distanza
			for (int i = 0; i < PossibleTiles.Num(); i++)
			{
				int MDistance = PossibleTiles[i].GetManhattanDistance(XTargetTile, YTargetTile);
				if (MDistance < MinDistance)
				{
					MinDistance = MDistance;
					SelectedTile = PossibleTiles[i];
				}
			}
			break;
		}
		}
		
	}
	else
	{
		SelectedTile = PossibleTiles[0];
	}

	// Seleziono la direzione successiva
	NextDirection = SelectedTile.TileDirection;
	return GetWorldFromTile(SelectedTile.XTile, SelectedTile.YTile);
}

TArray<FTileSelection> AGrid::GetPossibleTiles(int XTile, int YTile, EDirection CurrentDir)
{
	TArray<FTileSelection> Selection;


	int XForward = XTile, XSide1 = XTile, XSide2 = XTile;
	int YForward = YTile, YSide1 = YTile, YSide2 = YTile;

	switch (CurrentDir)
	{
	case EDirection::Up:
	{
		GetTileInDir(CurrentDir, XForward, YForward);
		if (CanMoveToTile(XForward,YForward))
		{
			FTileSelection Sel(XForward, YForward, GetVectorDirection(CurrentDir));
			Selection.Add(Sel);
		}
		// SX
		GetTileInDir(EDirection::Left, XSide1, YSide1);
		if (CanMoveToTile(XSide1, YSide1))
		{
			FTileSelection Sel(XSide1, YSide1, GetVectorDirection(EDirection::Left));
			Selection.Add(Sel);
		}
		//DX
		GetTileInDir(EDirection::Right, XSide2, YSide2);
		if (CanMoveToTile(XSide2, YSide2))
		{
			FTileSelection Sel(XSide2, YSide2, GetVectorDirection(EDirection::Right));
			Selection.Add(Sel);
		}

		break;
	}
	case EDirection::Down:
	{
	
		GetTileInDir(EDirection::Left, XSide1, YSide1);
		if (CanMoveToTile(XSide1, YSide1))
		{
			FTileSelection Sel(XSide1, YSide1, GetVectorDirection(EDirection::Left));
			Selection.Add(Sel);
		}
		
		GetTileInDir(CurrentDir, XForward, YForward);
		if (CanMoveToTile(XForward, YForward))
		{
			FTileSelection Sel(XForward, YForward, GetVectorDirection(CurrentDir));
			Selection.Add(Sel);
		}
		
		GetTileInDir(EDirection::Right, XSide2, YSide2);
		if (CanMoveToTile(XSide2, YSide2))
		{
			FTileSelection Sel(XSide2, YSide2, GetVectorDirection(EDirection::Right));
			Selection.Add(Sel);
		}
		break;
	}
	case EDirection::Left:
	{
		
		GetTileInDir(EDirection::Up, XSide1, YSide1);
		if (CanMoveToTile(XSide1, YSide1))
		{
			FTileSelection Sel(XSide1, YSide1, GetVectorDirection(EDirection::Up));
			Selection.Add(Sel);
		}
		
		GetTileInDir(CurrentDir, XForward, YForward);
		if (CanMoveToTile(XForward, YForward))
		{
			FTileSelection Sel(XForward, YForward, GetVectorDirection(CurrentDir));
			Selection.Add(Sel);
		}
		
		GetTileInDir(EDirection::Down, XSide2, YSide2);
		if (CanMoveToTile(XSide2, YSide2))
		{
			FTileSelection Sel(XSide2, YSide2, GetVectorDirection(EDirection::Down));
			Selection.Add(Sel);
		}
		break;
	}
	case EDirection::Right:
	{
		
		GetTileInDir(EDirection::Up, XSide1, YSide1);
		if (CanMoveToTile(XSide1, YSide1))
		{
			FTileSelection Sel(XSide1, YSide1, GetVectorDirection(EDirection::Up));
			Selection.Add(Sel);
		}
		
		GetTileInDir(EDirection::Down, XSide2, YSide2);
		if (CanMoveToTile(XSide2, YSide2))
		{
			FTileSelection Sel(XSide2, YSide2, GetVectorDirection(EDirection::Down));
			Selection.Add(Sel);
		}
		
		GetTileInDir(CurrentDir, XForward, YForward);
		if (CanMoveToTile(XForward, YForward))
		{
			FTileSelection Sel(XForward, YForward, GetVectorDirection(CurrentDir));
			Selection.Add(Sel);
		}
		break;
	}
	default:
		break;
	}
	return Selection;
}

EDirection AGrid::GetDirection(const FVector & Dir)
{
	if (Dir.X != 0)
	{
		if (Dir.X < 0)
		{
			return EDirection::Left;
		}
		else
		{
			return EDirection::Right;
		}
	}
	else if (Dir.Y != 0)
	{
		if (Dir.Y < 0)
		{
			return EDirection::Up;
		}
		else
		{
			return EDirection::Down;
		}
	}

	return EDirection::Up;
}

FVector AGrid::GetVectorDirection(const EDirection & NewDirection)
{
	switch (NewDirection)
	{
	case EDirection::Up:
	{
		return FVector(0.0f, -1.0f, 0.0f);
		break;
	}
	case EDirection::Down:
	{
		return FVector(0.0f, 1.0f, 0.0f);
		break;
	}
	case EDirection::Right:
	{
		return FVector(1.0f, 0.0f, 0.0f);
		break;
	}
	case EDirection::Left:
	{
		return FVector(-1.0f, 0.0f, 0.0f);
		break;
	}
	default:
		break;
	}

	return FVector(0.0f, -1.0f, 0.0f);
}
