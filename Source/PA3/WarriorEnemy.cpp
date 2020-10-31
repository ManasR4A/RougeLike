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
		if (currentTileLoc.Y > playerTileLoc.Y)
		{
			return (TryMoveInDir(North) ? true : false);
		}

		if (currentTileLoc.Y < playerTileLoc.Y)
		{
			return (TryMoveInDir(South) ? true : false);
		}
	}

	if (UKismetMathLibrary::NearlyEqual_FloatFloat(currentTileLoc.Y, playerTileLoc.Y, 10.f))
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy, player in same Y axis"));
		// check east and west and move appropriately
		if (currentTileLoc.X < playerTileLoc.X)
		{
			return (TryMoveInDir(East) ? true : false);
		}

		if (currentTileLoc.X > playerTileLoc.X)
		{
			return (TryMoveInDir(West) ? true : false);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Enemy, player in differenct axes"));

	// if both axis different, move in suitable axis first
	bool bMovedInX = false, bMovedInY = false;
	if (currentTileLoc.Y > playerTileLoc.Y)
	{
		bMovedInY = TryMoveInDir(North) ? true : false;
	}

	else if (currentTileLoc.Y < playerTileLoc.Y)
	{
		bMovedInY = TryMoveInDir(South) ? true : false;
	}

	if (bMovedInY)
	{
		return bMovedInY;
	}
		

	if (currentTileLoc.X < playerTileLoc.X)
	{
		bMovedInX = TryMoveInDir(East) ? true : false;
	}

	else if (currentTileLoc.X > playerTileLoc.X)
	{
		bMovedInX = TryMoveInDir(West) ? true : false;
	}

	return (bMovedInX);

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
