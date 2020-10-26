// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "TileType.h"

#include "GameFramework/PlayerController.h"
#include "PA3PlayerController.generated.h"


UCLASS()
class APA3PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APA3PlayerController();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	/** True if the charecter is jumping. */
	uint32 bJumping : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	/** Input handlers for Jump action. */
	void OnJumpPressed();
	void OnJumpReleased();

	/** Helper function to manage movement of player and respective tile data handling.*/
	void MovePlayerToTile(class UTileComponent* i_TargetTile);
	
	/** Helper function to rotate the player ro face the target tile. */
	void FacePlayerToTile(class UTileComponent* i_TargetTile);

	/** Helper function to Get players facing direction in EDoorOrientation format. */
	TEnumAsByte<EDoorOrientation> GetPlayerRotationCardinal();

	bool CheckNextTileInDir(UTileComponent* i_currentTile, TEnumAsByte<EDoorOrientation> i_Dir);

};


