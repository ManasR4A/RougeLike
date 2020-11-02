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

	// same as base movement
	return Super::MoveToPlayer(i_playerChar);

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

