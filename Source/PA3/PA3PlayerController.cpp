// Copyright Epic Games, Inc. All Rights Reserved.

#include "PA3PlayerController.h"
#include "TileComponent.h"
#include "DoorComponent.h"
#include "GameManager.h"

#include "Kismet/KismetMathLibrary.h"
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

	InputComponent->BindAction("Jump", IE_Pressed, this, &APA3PlayerController::OnJumpPressed);

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
			if (playerTile->adjecentTiles.FindKey(HitDoor->parentTile))
			{
				playerChar->gameManagerRef->bPlayersTurn = false;
				MovePlayerToTile(HitDoor->parentTile);

				// if door is connected to other door, update player and move player
				UDoorComponent* connectedDoor = HitDoor->connectedDoor;
				if (connectedDoor)
				{
					MovePlayerToTile(connectedDoor->parentTile);
					playerChar->gameManagerRef->PlayerInNewRoom(playerChar->currentRoom, connectedDoor->parentTile->parentRoom);
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

			UTileComponent* playerTile = playerChar->currentTile;

			if (playerTile != HitTile)
			{
				if (playerTile->adjecentTiles.FindKey(HitTile) && !HitTile->Visitor && HitTile->tileType != Wall && (HitTile->tileType != Lava || playerChar->FireProtection))
				{
					MovePlayerToTile(HitTile);
					playerChar->gameManagerRef->bPlayersTurn = false;
				}
				else
				{
					FacePlayerToTile(HitTile);
					UE_LOG(LogTemp, Warning, TEXT("Cannot move to THAT tile."));
				}
					

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
	auto playerChar = Cast<APA3Character>(GetCharacter());
	if (true)//playerChar->gameManagerRef->bPlayersTurn)
	{
		MoveToMouseCursor();
	}
}

void APA3PlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void APA3PlayerController::OnJumpPressed()
{
	bJumping = true;

	APA3Character* playerChar = Cast<APA3Character>(GetCharacter());
	auto playerFacing = GetPlayerRotationCardinal();

	// only execute if the player has enough Mana
	if (playerChar->Mana > playerChar->gameManagerRef->ManaCosts[Jump])
	{
		UTileComponent* currTile = playerChar->currentTile;
		// check the adjesent tile if exists and in the direction.
		for (int32 jumpAmount = 0; jumpAmount < playerChar->JumpRadius; jumpAmount++)
		{
			if (CheckNextTileInDir(currTile, playerFacing))
			{
				currTile = currTile->adjecentTiles[playerFacing];
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Cannot Cross Gap."))
				currTile = nullptr;
				break;
			}
		}

		// if the loop did not break and the final currTile is not Lava or does not have Visitor
		if (currTile && currTile->tileType != Lava && !currTile->Visitor)
		{
			MovePlayerToTile(currTile);
			playerChar->Mana -= 2;
		}
	}
}

void APA3PlayerController::OnJumpReleased()
{
	bJumping = false;
}

void APA3PlayerController::MovePlayerToTile(UTileComponent* i_TargetTile)
{
	APA3Character* playerChar = Cast<APA3Character>(GetCharacter());
	
	// For Rotation
	FVector distVector = i_TargetTile->GetOwner()->GetActorLocation();
	distVector.Z = 0.f;
	FVector playerLoc = playerChar->GetActorLocation();
	playerLoc.Z = 0.f;
	distVector = distVector - playerLoc;
	FRotator oldRotation = playerChar->GetActorRotation();
	FRotator newRotation = distVector.Rotation();
	newRotation.Pitch = oldRotation.Pitch;
	newRotation.Roll = oldRotation.Roll;

	playerChar->currentTile->Visitor = nullptr;

	// Updating Location and Rotation
	playerChar->SetActorLocation(i_TargetTile->GetOwner()->GetActorLocation(), true);
	playerChar->SetActorRotation(newRotation);

	playerChar->currentTile = i_TargetTile;
	i_TargetTile->Visitor = playerChar;
}

void APA3PlayerController::FacePlayerToTile(UTileComponent* i_TargetTile)
{
	APA3Character* playerChar = Cast<APA3Character>(GetCharacter());

	// For Rotation
	FVector distVector = i_TargetTile->GetOwner()->GetActorLocation();
	distVector.Z = 0.f;
	FVector playerLoc = playerChar->GetActorLocation();
	playerLoc.Z = 0.f;
	distVector = distVector - playerLoc;
	FRotator oldRotation = playerChar->GetActorRotation();
	FRotator newRotation = distVector.Rotation();
	newRotation.Pitch = oldRotation.Pitch;
	newRotation.Roll = oldRotation.Roll;

	playerChar->SetActorRotation(newRotation);
}

TEnumAsByte<EDoorOrientation> APA3PlayerController::GetPlayerRotationCardinal()
{
	TEnumAsByte<EDoorOrientation> ret;

	float rawYaw = Cast<APA3Character>(GetCharacter())->GetActorRotation().Yaw;
	bool bEast = UKismetMathLibrary::InRange_FloatFloat(rawYaw, -45.f, 45.f, false);
	bool bSouth = UKismetMathLibrary::InRange_FloatFloat(rawYaw, 45.f, 135.f, false);
	bool bWest = UKismetMathLibrary::InRange_FloatFloat(rawYaw, 135.f, 180.f, false) || UKismetMathLibrary::InRange_FloatFloat(rawYaw, -180.f, -135.f);
	bool bNorth = UKismetMathLibrary::InRange_FloatFloat(rawYaw, -135.f, -45.f, false);

	if (bEast) ret = East;
	else if (bSouth) ret = South;
	else if (bWest) ret = West;
	else if (bNorth) ret = North;

	return ret;
}

bool APA3PlayerController::CheckNextTileInDir(UTileComponent* i_currentTile, TEnumAsByte<EDoorOrientation> i_Dir)
{
	if (i_currentTile->adjecentTiles.Contains(i_Dir))
	{
		UTileComponent* nextTile = i_currentTile->adjecentTiles[i_Dir];

		// next tile in the dir should not be wall or door
		if (nextTile->tileType != Wall && nextTile->tileType != Door)
		{
			return true;
		}
	}
	return false;
}
