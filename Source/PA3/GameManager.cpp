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
}

void AGameManager::PostTileBoardGeneration()
{
	currentRoom = tileBoard->rooms[0];
	playerCharecterRef->currentRoom = tileBoard->rooms[0];
	playerCharecterRef->currentTile = currentRoom->startTile;
	playerCharecterRef->currentTile->Visitor = playerCharecterRef;
	playerCharecterRef->SetActorLocation(playerCharecterRef->currentTile->GetWorldLocation(WorldScaler));
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	playerCharecterRef = Cast<APA3Character>(GetWorld()->GetFirstPlayerController()->GetCharacter());
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (playerCharecterRef->currentTile->tileType == Victory)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), Cast<APlayerController>(playerCharecterRef->GetController()), EQuitPreference::Quit, true);
	}

	// testing
	currentRoom = playerCharecterRef->currentRoom;
}

