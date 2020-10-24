// Copyright Epic Games, Inc. All Rights Reserved.

#include "PA3PlayerController.h"
#include "TileComponent.h"
#include "DoorComponent.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "PA3Character.h"
#include "Engine/World.h"

APA3PlayerController::APA3PlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void APA3PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	//if (bMoveToMouseCursor)
	//{
	//	MoveToMouseCursor();
	//}
}

void APA3PlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &APA3PlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &APA3PlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APA3PlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &APA3PlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &APA3PlayerController::OnResetVR);
}

void APA3PlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void APA3PlayerController::MoveToMouseCursor()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (APA3Character* MyPawn = Cast<APA3Character>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		UTileComponent* HitTile = Hit.Actor.Get()->FindComponentByClass<UTileComponent>();
		UDoorComponent* HitDoor = Hit.Actor.Get()->FindComponentByClass<UDoorComponent>();

		if (HitDoor)
		{
		auto playerChar = Cast<APA3Character>(GetCharacter());
		UTileComponent* playerTile = playerChar->currentTile;

		if (playerTile != HitDoor->parentTile)
		{
			if (playerTile->adjecentTiles.Contains(HitDoor->parentTile))
			{
				playerChar->SetActorLocation(HitDoor->GetOwner()->GetActorLocation());
				playerChar->currentTile = HitDoor->parentTile;

				// if door is connected to other door, update player ad move player
				UDoorComponent* connectedDoor = HitDoor->connectedDoor;
				if (connectedDoor)
				{
					playerChar->SetActorLocation(connectedDoor->GetOwner()->GetActorLocation());
					playerChar->currentTile = connectedDoor->parentTile;
					playerChar->currentRoom = connectedDoor->parentTile->parentRoom;
				}
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("Door Not Adjesent."));
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Same Door as Current Door"));

		}

		else if (HitTile)
		{
			auto playerChar = Cast<APA3Character>(GetCharacter());
			if (!playerChar)
			{
				UE_LOG(LogTemp, Error, TEXT("Cast to player char failed."));
				return;
			}
			UTileComponent* playerTile = playerChar->currentTile;

			if (playerTile != HitTile)
			{
				if (playerTile->adjecentTiles.Contains(HitTile) && !HitTile->Visitor)
				{
					playerChar->SetActorLocation(HitTile->GetOwner()->GetActorLocation());

					playerChar->currentTile->Visitor = nullptr;
					playerChar->currentTile = HitTile;
					playerChar->currentTile->Visitor = playerChar;
				}
				else
					UE_LOG(LogTemp, Warning, TEXT("Cannot move to THAT tile."));

			}
			else
				UE_LOG(LogTemp, Warning, TEXT("Same Tile as Current Tile"));
		}

		else
			UE_LOG(LogTemp, Error, TEXT("Didn't hit a tile"));

	}
}

void APA3PlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void APA3PlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 10.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void APA3PlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
	MoveToMouseCursor();
}

void APA3PlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}
