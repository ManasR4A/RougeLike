// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherEnemy.h"
#include "PA3Character.h"
#include "TileComponent.h"


AArcherEnemy::AArcherEnemy()
{
    this->Health = 1;
    this->EnemyType = Archer;
}

bool AArcherEnemy::MoveToPlayer(APA3Character* i_playerChar)
{
    // get player location and current tile
	FVector playerTileLoc = i_playerChar->currentTile->GetOwner()->GetActorLocation();
	UTileComponent* playerTile = i_playerChar->currentTile;

	// if player adjesent, move away
	TEnumAsByte<EDoorOrientation> MoveDir, AltDir;
	auto playerInDir = currentTile->adjecentTiles.FindKey(playerTile);
	
	if (playerInDir)
	{
		switch ((*playerInDir))
		{
		case North:
			MoveDir = South;
			AltDir = East;
			break;

		case South:
			MoveDir = North;
			AltDir = East;
			break;

		case East:
			MoveDir = West;
			AltDir = North;
			break;

		case West:
			MoveDir = East;
			AltDir = North;
			break;
		}

		// try move away
		bool bMoved = TryMoveInDir(MoveDir);
		if (!bMoved)
			bMoved = TryMoveInDir(AltDir);

		return bMoved;
	}

	// otherwise move towards player like Base
	return Super::MoveToPlayer(i_playerChar);

}

bool AArcherEnemy::CanAttackPlayer(APA3Character* i_playerChar)
{
    // get player tile and current tile
	UTileComponent* playerTile = i_playerChar->currentTile;
	FVector playerTileLoc = i_playerChar->currentTile->GetOwner()->GetActorLocation();
	FVector thisTileLoc = this->currentTile->GetOwner()->GetActorLocation();

	// if the player is not im the same x or y axes, the archer cannot attack the player
	if (playerTileLoc.X != thisTileLoc.X && playerTileLoc.Y != thisTileLoc.Y)
	{
		return false;
	}

	bool bPlayerToNorth = FindPlayerInDir(i_playerChar, North);
	if (bPlayerToNorth)
	{
		return bPlayerToNorth;
	}
	
	bool bPlayerToSouth = FindPlayerInDir(i_playerChar, South);
	if (bPlayerToSouth)
	{
		return bPlayerToSouth;
	}

	bool bPlayerToEast = FindPlayerInDir(i_playerChar, East);
	if (bPlayerToEast)
	{
		return bPlayerToEast;
	}

	bool bPlayerToWest = FindPlayerInDir(i_playerChar, West);
	if (bPlayerToWest)
	{
		return bPlayerToWest;
	}

	return false;

}

bool AArcherEnemy::TakeTurn(APA3Character* i_playerChar)
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

bool AArcherEnemy::FindPlayerInDir(APA3Character* i_playerCharRef, TEnumAsByte<EDoorOrientation> i_Dir)
{
	UTileComponent** visitingTile = currentTile->adjecentTiles.Find(i_Dir);
	if (visitingTile && (*visitingTile)->tileType != Wall && (*visitingTile)->tileType != Door)
	{
		UTileComponent** targetTile = (*visitingTile)->adjecentTiles.Find(i_Dir);
		if (targetTile && (*targetTile) == i_playerCharRef->currentTile)
		{
			return true;
		}
	}
	return false;
}
