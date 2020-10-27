// Fill out your copyright notice in the Description page of Project Settings.

#include "WarriorEnemy.h"
#include "TileComponent.h"
#include "TileType.h"
#include "PA3Character.h"

#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

AWarriorEnemy::AWarriorEnemy()
{
	this->EnemyType = Warrior;
	this->Health = 1;
}

//void AWarriorEnemy::MoveToTile(UTileComponent* i_TargetTile, bool bTeleport)
//{
//	this->currentTile->Visitor = nullptr;
//
//	// for location
//	FVector oldLoc = this->GetActorLocation();
//	FVector newLoc = i_TargetTile->GetOwner()->GetActorLocation();
//
//	if (true)
//	{
//		this->SetActorLocation(newLoc);
//		// for rotation
//		oldLoc.Z = 0.f;
//		newLoc.Z = 0.f;
//		FRotator oldRot = this->GetActorRotation();
//		FVector DifVector = newLoc - oldLoc;
//		FRotator newRot = DifVector.Rotation();
//		newRot.Roll = oldRot.Roll;
//		newRot.Pitch = oldRot.Pitch;
//		this->SetActorRotation(newRot);
//	}
//	else
//	{
//		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), newLoc);
//	}
//
//	i_TargetTile->Visitor = this;
//	this->currentTile = i_TargetTile;
//}

bool AWarriorEnemy::MoveToPlayer(APA3Character* i_playerChar)
{
	// Get the tile currentTile locations
	FVector currentTileLoc = currentTile->GetOwner()->GetActorLocation();
	FVector playerTileLoc = i_playerChar->currentTile->GetOwner()->GetActorLocation();

	// check in both axes if same or not
	// if same in some axis, move towards player in other axis.
	if (UKismetMathLibrary::NearlyEqual_FloatFloat(currentTileLoc.X, playerTileLoc.X, 10.f))
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy, player in same X axis"));
		// check north and south and move appropriately
		if (TryMoveInDir(North))
		{
			return true;
		}

		if (TryMoveInDir(South))
		{
			return true;
		}
	}

	if (UKismetMathLibrary::NearlyEqual_FloatFloat(currentTileLoc.Y, playerTileLoc.Y, 10.f))
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy, player in same Y axis"));
		// check east and west and move appropriately
		if (TryMoveInDir(East))
		{
			return true;
		}

		if (TryMoveInDir(West))
		{
			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Enemy, player in differenct axes"));

	// if both axis different, move in suitable axis first
	if (TryMoveInDir(North))
	{
		return true;
	}

	if (TryMoveInDir(East))
	{
		return true;
	}

	if (TryMoveInDir(South))
	{
		return true;
	}

	if (TryMoveInDir(West))
	{
		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("warrior Not Moved."));
	return false;

}

bool AWarriorEnemy::CanAttackPlayer(APA3Character* i_playerChar)
{
	if (i_playerChar->currentTile->adjecentTiles.FindKey(currentTile))
	{
		return true;
	}
	return false;
}

bool AWarriorEnemy::TakeTurn(APA3Character* i_playerChar)
{
	if (CanAttackPlayer(i_playerChar))
	{
		i_playerChar->DamagePlayer();
	}

	else
	{
		bool bMovedToPlayer = MoveToPlayer(i_playerChar);
	}
	UE_LOG(LogTemp, Warning, TEXT("warrior Taken Turn."));

	return true;

}

bool AWarriorEnemy::TryMoveInDir(TEnumAsByte<EDoorOrientation> i_dir)
{
	UTileComponent ** nextTile = currentTile->adjecentTiles.Find(i_dir);
	if (nextTile && (*nextTile)->tileType == Floor && !(*nextTile)->Visitor)
	{
		MoveToTile((*nextTile), false);
		return true;
	}
	return false;
}
