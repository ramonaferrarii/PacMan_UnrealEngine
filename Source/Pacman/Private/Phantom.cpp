#include "Phantom.h"
#include "PacmanActor.h"
#include "PacmanGameModeBase.h"
#include "Components/SphereComponent.h"
#include <Kismet/GameplayStatics.h>
#include "Sound/SoundWave.h"
#include "DrawDebugHelpers.h"

// Costruttore della classe APhantom che imposta il flag bCanEverTick su true. 
// Viene creato un componente "PhantomMesh". 
APhantom::APhantom()
{
	PrimaryActorTick.bCanEverTick = true;
	PhantomMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PhantomMesh"));
	PhantomMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	RootComponent = PhantomMesh;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.5f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetupAttachment(RootComponent);
}

// Chiamato quando il gioco comincia
void APhantom::BeginPlay()
{
	Super::BeginPlay();
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APhantom::OnOverlap);
	GetWorld()->GetTimerManager().SetTimer(ScatterTimerHandle, this, &APhantom::OnScatterTimerExpired, ScatterDuration, false);
}

void APhantom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Grid)
	{
	    int XTileActor, YTileActor = 0;
		Grid->GetTileFromWorld(GetActorLocation(), XTileActor, YTileActor);
		
		if (XTileActor == XTileDestination && YTileActor == YTileDestination)
		{

			SetActorLocation(Destination);

			// Teletrasporto
			if (Grid->GetTileValue(XTileActor, YTileActor) == 'T')
			{
				if (YTileActor == 0)
					SetActorLocation(Grid->GetGridSpecialPosition(RightTeleport));
				else
					SetActorLocation(Grid->GetGridSpecialPosition(LeftTeleport));
			}

			// Mi da la prossima destinazione e il prossimo movimento
			Destination = Grid->GetNextDestination( GetTargetPosition(), GetActorLocation(), MovementDir, NextMovementDir);
			Grid->GetTileFromWorld(Destination, XTileDestination, YTileDestination);

			// Cambio direzione
			MovementDir = NextMovementDir;
		}

		// Movimento
		FVector NextPosition = GetActorLocation() + (MovementDir * (Speed * SpeedMultiplier * DeltaTime));
		NextPosition.Z = 0;
		SetActorLocation(NextPosition, true);
	}

	if (GetState() == EState::Dead)
	{
		int XTileDead = 0, YTileDead = 0;
		Grid->GetTileFromWorld(GetActorLocation(), XTileDead, YTileDead);

		if (XTileDead == 11 && YTileDead == 13)
			ChangeState(EState::Chase);
	}
}

void APhantom::SetGrid(AGrid * NewGrid)
{
	Grid = NewGrid;
	Destination = Grid->GetNextDestination(GetTargetPosition(), GetActorLocation(), MovementDir, NextMovementDir);
	Grid->GetTileFromWorld(Destination, XTileDestination, YTileDestination);
}

EState APhantom::GetState()
{
	return CurrentState;
}

void APhantom::ChangeState(EState NewState)
{
	if (NewState != CurrentState)
	{
		CurrentState = NewState;
	}
}

void APhantom::SetSpeedMultiplier(float Value)
{
	SpeedMultiplier = Value;
}

void APhantom::ChangeScatterPosition(EGridPositions NewPosition)
{
	ScatterPosition = NewPosition;
}

void APhantom::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(APacmanActor::StaticClass())) {
		APacmanActor* Pacman = Cast<APacmanActor>(OtherActor);

		if (!Pacman->IsPacmanInvincible()) {
			UGameplayStatics::PlaySound2D(this, minuslife);
			APacmanGameModeBase* GameMode = Cast<APacmanGameModeBase>(GetWorld()->GetAuthGameMode());
			GameMode->Lives--;
		}
	}
}


void APhantom::ChangeDirection()
{
	if (Grid)
	{
		MovementDir *= -1.0f;
		Destination = Grid->GetNextDestination(GetTargetPosition(), GetActorLocation(), MovementDir, NextMovementDir);
		Grid->GetTileFromWorld(Destination, XTileDestination, YTileDestination);
	}
}

void APhantom::OnScatterTimerExpired()
{
	ChangeState(EState::Chase);
}

FVector APhantom::GetTargetPosition()
{
	switch (CurrentState)
	{
	case EState::Chase:
	{
		SetSpeedMultiplier(1.0f);
		return Grid->GetGridSpecialPosition(EGridPositions::PlayerPosition);
		break;
	}
	case EState::Scatter:
	{
		SetSpeedMultiplier(1.0f);
		return Grid->GetGridSpecialPosition(ScatterPosition);
		break;
	}
	case EState::Frightened:
	{
		if (Grid)
		{
			SetSpeedMultiplier(0.7f);
			return Grid->GetGridSpecialPosition(GhostsHouse);
		}
		break;
	}
	case EState::Dead:
	{
		if (Grid)
		{
			SetSpeedMultiplier(3.0f);
			return Grid->GetGridSpecialPosition(GhostsHouse);
		}
		break;
	}
	default:
		break;
	}
	return FVector();
}
