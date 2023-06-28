#include "PacmanActor.h"
#include "Components/SphereComponent.h"
#include "Point.h"
#include "Phantom.h"
#include "PacmanGrid.h"
#include "Components/StaticMeshComponent.h"
#include <Kismet/GameplayStatics.h>
#include "Sound/SoundWave.h"
#include "DrawDebugHelpers.h"



// Setto i valori di default
APacmanActor::APacmanActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	RootComponent = PlayerMesh;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.5f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetupAttachment(RootComponent);
	
}

// Chiamato quando il gioco inizia
void APacmanActor::BeginPlay()
{
	Super::BeginPlay();
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APacmanActor::OnOverlap);
}

void APacmanActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bStopMovement)
	{
		bool bCanMove = Grid->CalculateNewDestination(CurrentDirection, Destination);
		// Se la differenza tra le destinazione e la posizione in cui sono è meno di 1 allora mi fermo, altrimenti no e continuo ad andare avanti
		FVector NextPosition = GetActorLocation() + (MovementDirection * (Speed * SpeedMultiplier * DeltaTime));
		NextPosition.Z = 0.0f;
		FVector Diff = (Destination - NextPosition) * MovementDirection;
		if (!bCanMove && (FMath::Abs(Diff.Size()) < ThresholdRadius))
		{
			// Fine del movimento
			SetActorLocation(Destination, true);
			bStopMovement = true;
		}
		else
		{
			SetActorLocation(NextPosition, true);
		}

		// Teletrasporto
		int XTile, YTile = 0;
		Grid->GetTileFromWorld(GetActorLocation(), XTile, YTile);

		if (XTile == 14 && YTile == 0)
		{
			SetActorLocation(Grid->GetGridSpecialPosition(RightTeleport));
		}

		if (XTile == 14 && YTile == 28)
		{
			SetActorLocation(Grid->GetGridSpecialPosition(LeftTeleport));
		}
	}
}

void APacmanActor::SetMovement(EDirection NewDirection)
{
	if (NewDirection != CurrentDirection && Grid && Grid->CanPlayerMoveInDir(NewDirection))
	{
		bStopMovement = false;
		CurrentDirection = NewDirection;
		MovementDirection = Grid->GetVectorDirection(CurrentDirection);
	}
}

bool APacmanActor::IsPacmanInvincible()
{
	return PacmanIsInvincible;
}

void APacmanActor::OnSpeedBoostTimerExpired()
{
	SpeedMultiplier = 1.0f;
	PacmanIsInvincible = false;

	TArray<AActor*> Phantoms;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APhantom::StaticClass(), Phantoms);
	for (AActor* Actor : Phantoms)
	{
		APhantom* Phantom = Cast<APhantom>(Actor);
		if (Phantom)
		{
			if(Phantom->GetState() != EState::Dead)
				Phantom->ChangeState(PhantomStateBeforeFrightened);
		}
	}
}

void APacmanActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(APoint::StaticClass()))
	{
		APoint* Point = Cast<APoint>(OtherActor);
		Point->Eat();
		if (Point->GetSpecial()) {
			SpeedMultiplier = 2.0f;
			PacmanIsInvincible = true;

			TArray<AActor*> Phantoms;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APhantom::StaticClass(), Phantoms);
			for (AActor* Actor : Phantoms)
			{
				APhantom* Phantom = Cast<APhantom>(Actor);
				if (Phantom)
				{
					PhantomStateBeforeFrightened = Phantom->GetState();
					Phantom->ChangeState(EState::Frightened);
				}
			}

			GetWorld()->GetTimerManager().SetTimer(SpeedBoostTimerHandle, this, &APacmanActor::OnSpeedBoostTimerExpired, SpeedBoostDuration, false);
			
		}
	}
	else if (OtherActor->IsA(APhantom::StaticClass()))
	{
		APhantom* Phantom = Cast<APhantom>(OtherActor);
		if (PacmanIsInvincible)
		{
			UGameplayStatics::PlaySound2D(this, eat_ghost);
			PhantomStateBeforeDead = Phantom->GetState();
			Phantom->ChangeState(EState::Dead);
		}

	}

}


