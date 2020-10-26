// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"

#include "PA3Character.h"
#include "TileBoardComponent.h"
#include "TileComponent.h"
#include "RoomComponent.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bTileBoardGenerated = false;
	bTilesAddressed = false;
	bGenerationComplete = false;

	MaxMana = 5;
	MaxHealth = 3;
	MaxJumpRadius = 2;
	MinThrowRadius = 2;
	MaxThrowRadius = 5;

}

void AGameManager::PostTileBoardGeneration()
{
	currentRoom = tileBoard->rooms[0];
	currentDeapth = currentRoom->roomDeapth;

	playerCharecterRef->gameManagerRef = this;
	playerCharecterRef->currentRoom = tileBoard->rooms[0];
	playerCharecterRef->currentTile = currentRoom->startTile;
	playerCharecterRef->currentTile->Visitor = playerCharecterRef;
	playerCharecterRef->Health = MaxHealth;
	playerCharecterRef->Mana = MaxMana;
	playerCharecterRef->JumpRadius = MaxJumpRadius;
	playerCharecterRef->ThrowRaius = MinThrowRadius;

	playerCharecterRef->SetActorLocation(playerCharecterRef->currentTile->GetWorldLocation(WorldScaler));
}

void AGameManager::PlayerInNewRoom(URoomComponent* i_OldRoom, URoomComponent* i_NewRoom)
{
	playerCharecterRef->currentRoom = i_NewRoom;
	playerCharecterRef->Mana = MaxMana;

}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Quit if player enters victory tile
	if (playerCharecterRef->currentTile->tileType == Victory)
	{
		bVictory = true;
		//UKismetSystemLibrary::QuitGame(GetWorld(), Cast<APlayerController>(playerCharecterRef->GetController()), EQuitPreference::Quit, true);
	}

	if (playerCharecterRef->Health <= 0)
	{
		bFailure = true;
	}

	// testing
	currentRoom = playerCharecterRef->currentRoom;
	currentDeapth = currentRoom->roomDeapth;
}

