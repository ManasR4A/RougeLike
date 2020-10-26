// Fill out your copyright notice in the Description page of Project Settings.
#include "TileBoardGenerator.h"
#include "RoomComponent.h"
#include "DoorComponent.h"
#include "VictoryComponent.h"
#include "WallComponent.h"
#include "GameManager.h"
#include "PA3Character.h"

#include "Kismet/KismetMathLibrary.h"
#include "GenericPlatform/GenericPlatformMath.h"


// Sets default values
ATileBoardGenerator::ATileBoardGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LavaGenMarkovMatrix.Add(Lava, FVector2D::ZeroVector);
	LavaGenMarkovMatrix.Add(Floor, FVector2D::ZeroVector);
	
}

// Called when the game starts or when spawned
void ATileBoardGenerator::BeginPlay()
{
	Super::BeginPlay();

	if(seed)
		FGenericPlatformMath::SRandInit(seed);
	else
	{
		int randSeed = FGenericPlatformMath::SRand() * 3141598.f;
		FGenericPlatformMath::SRandInit(randSeed);
	}
	Generate();
	
	m_totalGeneratedRooms = tileBoard->rooms.Num();
	gameManagerRef->bTileBoardGenerated = true;
	gameManagerRef->tileBoard = tileBoard;
	gameManagerRef->playerCharecterRef = Cast<APA3Character>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	gameManagerRef->PostTileBoardGeneration();
}

int32 ATileBoardGenerator::GenerateLavaTilesInRoom(URoomComponent* i_TargetRoom)
{
	int32 lavaTileCount = 0;
	int32 spawnTries = 0;

	// Getting Lava Gen Start Tile
	UTileComponent* selectedTile = nullptr;

	for (auto tile : i_TargetRoom->tiles)
	{
		if (tile->tileType == Floor && !tile->Visitor)
		{
			for (auto adjTile : tile->adjecentTiles)
			{
				if (adjTile.Value->tileType != Door && !adjTile.Value->Visitor)
				{
					selectedTile = tile;
					break;
				}
			}
			if (selectedTile)
				break;
		}
	}

	if (!selectedTile)
		return 0;

	int32 tilesVisited = 0;
	int32 totalTilesToVisit = UKismetMathLibrary::FFloor(i_TargetRoom->tiles.Num() * LavaTilePercent);
	
	UTileComponent* nextTile;

	// small number of iterations to limit total iterations
	while (tilesVisited < totalTilesToVisit)
	{
		TArray<TEnumAsByte<EDoorOrientation>> keys;
		selectedTile->adjecentTiles.GetKeys(keys);
		int32 randAdjesentTileINdex = UKismetMathLibrary::RandomIntegerInRange(0, keys.Num() - 1);

		nextTile = selectedTile->adjecentTiles[keys[randAdjesentTileINdex]];
		bool nexTileNextToDoor = false;

		// check if the nextTile is next to a door or not.
		for (auto adjTile : nextTile->adjecentTiles)
		{
			if (adjTile.Value->tileType == Door)
			{
				nexTileNextToDoor = true;
				break;
			}
		}

		// check if the nextTile is LavaPossible
		if (nextTile->tileType != Door && !nextTile->Visitor && !nexTileNextToDoor)
		{
			tilesVisited++;

			// get the probabilities
			auto lookupKey = selectedTile->tileType;
			FVector2D probabilities = *(LavaGenMarkovMatrix.Find(lookupKey));

			// roll dice and check
			float spawnChance = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f);
			if (spawnChance <= probabilities.X)
			{
				nextTile->MakeLavaTile(LavaMat);
				selectedTile = nextTile;
				lavaTileCount++;
			}
		}
	}
	

	return lavaTileCount;
}

bool ATileBoardGenerator::GenerateUpgradeTileInRoom(URoomComponent* i_TargetRoom)
{
	int32 Upgradetries = 0;
	while (!i_TargetRoom->bHasUpgradeTile && Upgradetries < maxTries)
	{
		Upgradetries++;
		int32 randTileIndex = UKismetMathLibrary::RandomIntegerInRange(0, i_TargetRoom->tiles.Num() - 1);
		UTileComponent* selectedTile = i_TargetRoom->tiles[randTileIndex];
		if (selectedTile->tileType == Floor && !selectedTile->Visitor)
		{
			selectedTile->MakeUpgradeTile(UpgradeMat);
			i_TargetRoom->bHasUpgradeTile = true;
			return true;
		}
	}
	return false;
}

int32 ATileBoardGenerator::GenerateEnemiesInRoom(URoomComponent* i_TargetRoom)
{
	return int32();
}

TEnumAsByte<EDoorOrientation> ATileBoardGenerator::GetOppositeSide(TEnumAsByte<EDoorOrientation> i_doorDir)
{
	switch (i_doorDir)
	{
	case North: return South;

	case East: return West;

	case West: return East;

	case South: return North;
	}

	return North;
}

int32 ATileBoardGenerator::hasOppositeDoorDIr(FString i_roomData, TEnumAsByte<EDoorOrientation> i_doorDir)
{
	switch (i_doorDir)
	{
	case North: return i_roomData.Find("S");

	case East: return i_roomData.Find("W");

	case West: return i_roomData.Find("E");

	case South: return i_roomData.Find("N");
	}
	return -1;
}

UDoorComponent* ATileBoardGenerator::GetFacingDoor(URoomComponent* i_roomToLookIn, TEnumAsByte<EDoorOrientation> i_dirToLooksFor)
{
	for (auto door : i_roomToLookIn->doors)
	{
		if (door->doorDirection == i_dirToLooksFor)
			return door;
	}
	return nullptr;
}

void ATileBoardGenerator::GenerateRoomAtDoor(UDoorComponent* i_DoorComponent, bool genVictoryRoom)
{
	currentDepth = i_DoorComponent->parentTile->parentRoom->roomDeapth + 1;
	// Generate Rooms at the Door Selected
	while (tries < maxTries)
	{
		//UE_LOG(LogTemp, Error, TEXT("Flowtest"));
		tries++;

		// get random value from seed
		int next;

		if (genVictoryRoom)
			next = FGenericPlatformMath::FloorToInt(FGenericPlatformMath::SRand() * victoryRoomNames.Num());
		else
			next = FGenericPlatformMath::FloorToInt(FGenericPlatformMath::SRand() * roomFileNames.Num());

		next = next == 0 ? 1 : next;

		// using random value, select a room to try and spawn
		FString roomToSpaw;
		if (genVictoryRoom)
			roomToSpaw = victoryRoomNames[next - 1];
		else
			roomToSpaw = roomFileNames[next - 1];
		FString roomData = LoadRoomString(roomToSpaw);
		int32 doorIndex = hasOppositeDoorDIr(roomData, i_DoorComponent->doorDirection);

		if (doorIndex == -1) // if no door suitable for current door in current room is found
		{
			// UE_LOG(LogTemp, Error, TEXT("No Suitable Door Found in the random room"))
				continue;
		}

		URoomComponent* spawnedRoom = CreateRoomComponentFromString(TEXT("Room"), roomData, FVector2D::ZeroVector);

		auto oppositeSide = GetOppositeSide(i_DoorComponent->doorDirection);
		UDoorComponent* doorToConnect = GetFacingDoor(spawnedRoom, oppositeSide);
		if (!doorToConnect) // will be true if something went wrong in spawning rooms and doors
			continue;

		allignRoomsViaDoors(currentRoom, i_DoorComponent, spawnedRoom, doorToConnect);

		bool roomsColliding = CheckForRoomCollision(spawnedRoom);
		if (roomsColliding)
		{
			DestroyRoom(spawnedRoom);
			continue;
		}

		// update connectedDoors
		i_DoorComponent->connectedDoor = doorToConnect;
		doorToConnect->connectedDoor = i_DoorComponent;

		// Finalize room and add to tileboard
		spawnedRoom->roomDeapth = currentDepth;
		tileBoard->rooms.Add(spawnedRoom);
		UE_LOG(LogTemp, Warning, TEXT("New room spawned."));
		break;
	}
	tries = 0;
}

bool ATileBoardGenerator::CheckForRoomCollision(URoomComponent* i_tempRoom)
{
	for (auto confirmedRoom : tileBoard->rooms)
	{
		for (auto tempTile : i_tempRoom->tiles)
		{
			FVector2D tempTileLoc = i_tempRoom->roomOrigin + (tempTile->roomLocation * worldScaler);
			for (auto permTile : confirmedRoom->tiles)
			{
				FVector2D permTileLOc = confirmedRoom->roomOrigin + (permTile->roomLocation * worldScaler);
				if (tempTileLoc == permTileLOc)
				{
					return true;
				}
			}
		}
	}
	return false;

}

void ATileBoardGenerator::allignRoomsViaDoors(URoomComponent* i_StaticRoom, UDoorComponent* i_StaticDoor, URoomComponent* i_RoomToMove, UDoorComponent* i_DoorToAllign)
{
	FVector Offset = (i_DoorToAllign->GetOwner()->GetActorLocation()) - (i_StaticDoor->GetOwner()->GetActorLocation());
	switch (i_StaticDoor->doorDirection)
	{
	case North: 
		Offset.Y += worldScaler;
		break;
	case East:
		Offset.X -= worldScaler;
		break;
	case West:
		Offset.X += worldScaler;
		break;
	case South:
		Offset.Y -= worldScaler;
	}

	FVector OGLoc = i_RoomToMove->GetOwner()->GetActorLocation();
	FVector NewLoc = OGLoc - Offset;
	i_RoomToMove->GetOwner()->SetActorLocation(NewLoc);

	i_RoomToMove->roomOrigin -= FVector2D(Offset.X, Offset.Y);

}

bool ATileBoardGenerator::DestroyRoom(URoomComponent* i_room)
{
	for (auto tile : i_room->tiles)
	{
		tile->GetOwner()->Destroy();
	}
	
	i_room->doors.Empty();
	i_room->tiles.Empty();
	i_room->boards.Empty();

	i_room->GetOwner()->Destroy();
	return true;
}

// Called every frame
void ATileBoardGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// don't tick if the initial tileboard ins't generated
	if (!gameManagerRef->bTileBoardGenerated)
	{
		return;
	}

	// go through the rooms (one room per frame) to address all the tiles within the room
	if (!gameManagerRef->bTilesAddressed && m_currentRoomIndex < m_totalGeneratedRooms)
	{
		currentRoom = tileBoard->rooms[m_currentRoomIndex];

		for (UTileComponent* tile : currentRoom->tiles)
		{
			// Update adjesent tiles
			int32 adjecenttilecount = GetAdjesentTiles(tile);

			// if the current tile is a door that is connected to other room, update both door tiles' adjesent tiles
			UDoorComponent* door = tile->GetOwner()->FindComponentByClass<UDoorComponent>();
			if (door && door->connectedDoor && !tile->adjecentTiles.FindKey(door->connectedDoor->parentTile))
			{
				auto doorDir = door->doorDirection;
				auto connectedDoorDir = GetOppositeSide(doorDir);
				tile->adjecentTiles.Add(doorDir, door->connectedDoor->parentTile);
				door->connectedDoor->parentTile->adjecentTiles.Add(connectedDoorDir, tile);
			}
		}

		// check for victoryRoom and attach VictoryTile
		if (currentRoom->roomDeapth == victoryDeapth)
		{
			UDoorComponent* door = currentRoom->doors[0];
			if (!door)
			{
				UE_LOG(LogTemp, Error, TEXT("Didn't find door in victory room."));
			}

			// getting the tile ref for the victory tile
			UTileComponent* victoryTile;
			for (auto tile : door->parentTile->adjecentTiles)
			{
				if (tile.Value->tileType != Door)
				{
					int32 randTileIndex = UKismetMathLibrary::RandomIntegerInRange(1, tile.Value->adjecentTiles.Num());
					victoryTile = tile.Value;// ->adjecentTiles[randTileIndex - 1];
					victoryTile->MakeVictoryTile(VictoryMat);
					UE_LOG(LogTemp, Warning, TEXT("VictoryTileCreated!"));
					break;
				}
			}
		}

		else if(currentRoom->roomDeapth < victoryDeapth)
		{
			int32 LavaTileCount = GenerateLavaTilesInRoom(currentRoom);
			bool UpgradeTileSpawned = GenerateUpgradeTileInRoom(currentRoom);
			//int32 EnemyCount = GenerateEnemiesInRoom(currentRoom);

			UE_LOG(LogTemp, Warning, TEXT("LavaTiles = %d"), LavaTileCount);
			if(UpgradeTileSpawned)
				UE_LOG(LogTemp, Warning, TEXT("UpgradeTile Spawned"));
			//UE_LOG(LogTemp, Warning, TEXT("Enemy Count = %d"), EnemyCount);
		}

		// to go to next room in the tile board
		m_currentRoomIndex++;

		// if all rooms were visited to address the tiles, update bTilesAddressed
		if (m_currentRoomIndex > m_totalGeneratedRooms)
		{
			gameManagerRef->bTilesAddressed = true;
		}
	}
}

void ATileBoardGenerator::Generate()
{
	// Generate the initial data and initial room.
	if (FirstRoom)
	{
		// Tile Board Generation
		FirstRoom = false;
		FRotator boardRot = GetActorRotation();
		FVector boardPos = GetActorLocation();
		AActor* boardActor = GetWorld()->SpawnActor(boardBP.Get(), &boardPos, &boardRot);
		if (!boardActor)
		{
			UE_LOG(LogTemp, Error, TEXT("Board Actor NOT Generated."));
			return;
		}

		tileBoard = boardActor->FindComponentByClass<UTileBoardComponent>();
		if (!tileBoard)
		{
			UE_LOG(LogTemp, Error, TEXT("Board NOT Found."));
			return;
		}

		#pragma region Spawn Initial Room

		FString roomName;
		if (StartingRoomFromArray <= roomFileNames.Num())
			roomName = roomFileNames[StartingRoomFromArray - 1];
		else
			roomName = roomFileNames[0];

		FString roomString = LoadRoomString(roomName);
		URoomComponent* startRoom = CreateRoomComponentFromString(FString("StartRoom"), roomString, FVector2D::ZeroVector);
		tileBoard->rooms.Add(startRoom);
		startRoom->roomDeapth = 0;

		#pragma endregion

		currentRoom = startRoom;
	}
	
	// Check and generation for victory room
	if (currentRoom->roomDeapth == victoryDeapth - 1)
	{
		auto prevRoom = currentRoom;
		if (prevRoom->doors.Num() > 1)
		{
			for (auto door : prevRoom->doors)
			{
				if (!door->connectedDoor)
				{
					VictoryRoom = true;
					GenerateRoomAtDoor(door, VictoryRoom);
				}
				currentRoom = prevRoom;
			}
		}
		return;
	}

	// get doors from current room & Geberate Rooms there

	//UDoorComponent* doorToAdvance;
	//int32 doorTries = 0;
	//while (!doorToAdvance && doorTries < maxTries)
	//{
	//	int32 randDoorINdex = UKismetMathLibrary::RandomIntegerInRange(0, currentRoom->doors.Num() - 1);
	//	if (!currentRoom->doors[randDoorINdex]->connectedDoor)
	//	{
	//		doorToAdvance = currentRoom->doors[randDoorINdex];
	//		GenerateRoomAtDoor(doorToAdvance, VictoryRoom);
	//	}
	//}

	else if (currentRoom->roomDeapth < victoryDeapth - 1)
	{
		for (auto door : currentRoom->doors)
		{
			int connectedCount = 0;

			// if selected door is already connected
			if (door->connectedDoor)
			{
				connectedCount++;
				if (connectedCount >= currentRoom->doors.Num())
				{
					currentDepth = currentRoom->roomDeapth;
					// UE_LOG(LogTemp, Warning, TEXT("All Door already connected."));
					continue;
				}

				// UE_LOG(LogTemp, Warning, TEXT("Door already connected."));
				continue;
			}

			// Generate Room
			GenerateRoomAtDoor(door, VictoryRoom);

		}
	}



	UDoorComponent* connectedDoor;
	int32 randDoorIndex;
	int32 doorSelectionTries = 0;

	// loop to get door until proper door is found in current room
	while (true && doorSelectionTries < maxTries)
	{
		doorSelectionTries++;
		randDoorIndex = UKismetMathLibrary::RandomIntegerInRange(0, currentRoom->doors.Num() - 1);
		connectedDoor = currentRoom->doors[randDoorIndex]->connectedDoor;

		// condition to check if the selected door is connected to a room forward in the room deapth 
		// and not the one before the current room/
		if (connectedDoor && connectedDoor->parentTile->parentRoom->roomDeapth > currentRoom->roomDeapth)
		{
			currentRoom = connectedDoor->parentTile->parentRoom;
			Generate();
			break;
		}
	}

	//for (auto door : prevRoom->doors)
	//{
	//	if (door->connectedDoor && currentRoom->roomDeapth == door->connectedDoor->parentTile->parentRoom->roomDeapth - 1)
	//	{
	//		count++;
	//		currentRoom = door->connectedDoor->parentTile->parentRoom;
	//		
	//		currentRoom = prevRoom;
	//	}
	//}
	//UE_LOG(LogTemp, Warning, TEXT("Count = %d"), count);
	

}

FString ATileBoardGenerator::LoadRoomString(FString RoomFileName)
{

	FString fileData;
	FString dir = FPaths::ProjectContentDir();
	dir.Append("Assets/Rooms/");

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *dir);
		
	FString filePath = dir.Append(RoomFileName);
	filePath.Append(".txt");
	

	IPlatformFile& fileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (fileManager.FileExists(*filePath))
	{
		if (FFileHelper::LoadFileToString(fileData, *filePath))
		{
			return fileData;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FileManagement: Unable to Load Room file. %s"), *filePath);
		}

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FileManagement: Room file not found. %s"), *filePath);
	}


	return FString("Not to be shown.");
}

URoomComponent* ATileBoardGenerator::CreateRoomComponentFromString(FString roomName, FString roomData, FVector2D roomOriginBoardPosition)
{
	// Get rot and pos
	FRotator roomRot = FRotator::ZeroRotator;
	FVector roomPos = FVector(worldScaler * roomOriginBoardPosition.X, worldScaler * roomOriginBoardPosition.Y, 0.f);

	// get actor and the component
	AActor* roomActor = GetWorld()->SpawnActor(roomBP.Get(), &roomPos, &roomRot);
	if (!roomActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Room Actor Spawm failed!"));
		return nullptr;
	}

	URoomComponent* room = roomActor->FindComponentByClass<URoomComponent>();
	if (!room)
	{
		UE_LOG(LogTemp, Error, TEXT("Room failed!"));
		return nullptr;
	}

	// attach room to tileBoard
	roomActor->AttachToActor(tileBoard->GetOwner(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Room"));

	// assign room params
	room->roomOrigin = roomOriginBoardPosition;
	room->roomName = roomName;
	//tileBoard->rooms.Add(room); //DO NOT ADD HERE. ONLY ADD AFTER SPAWNED
	
	// iterate through string and spawn room
	int x = 0, y = 0;
	for (int i = 0; i < roomData.Len(); i++)
	{
		FString tileChar = roomData.Mid(i, 1);

		if (tileChar == "\n")
		{
			x = 0;
			y++;
			continue;
		}

		// check for the tile char and set up params for spawning
		TEnumAsByte<ETileType> tileType;
		FRotator tileRotation = FRotator::ZeroRotator;

		if (tileChar == "N")
		{
			tileType = ETileType::Door;
			tileRotation = FRotator::ZeroRotator;
		}
		else if (tileChar == "E")
		{
			tileType = ETileType::Door;
			tileRotation = FRotator(0.f, 90.f, 0.f);
		}
		else if (tileChar == "S")
		{
			tileType = ETileType::Door;
			tileRotation = FRotator(0.f, 180.f, 0.f);
		}
		else if (tileChar == "W")
		{
			tileType = ETileType::Door;
			tileRotation = FRotator(0.f, 270.f, 0.f);
		}
		else if (tileChar == "|" || tileChar == "-")
		{
			tileType = ETileType::Wall;
			tileRotation = FRotator::ZeroRotator;
		}
		else if (tileChar == " ")
		{
			tileType = ETileType::Floor;
			tileRotation = FRotator::ZeroRotator;
		}
		else if (tileChar == "x")
		{
			x++;
			continue;
		}
		else if (tileChar == "\r")
		{
			continue;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Symbol in room Data %s"), *tileChar);
			continue;
		}

		SpawnTile(tileType, room, FVector2D(x, y), tileRotation);

		x++;
	}

	return room;

}

int32 ATileBoardGenerator::GetAdjesentTiles(UTileComponent* i_Tile)
{
	URoomComponent* parentRoom = i_Tile->parentRoom;

	// get i_Tile coords
	FVector tileCoords = FVector((parentRoom->roomOrigin + (i_Tile->roomLocation * worldScaler)), 0.f);

	// get all 4 direction coords
	FVector up, down, left, right;
	up = tileCoords + FVector(0.f, -worldScaler, 0.f);
	down = tileCoords + FVector(0.f, worldScaler, 0.f);
	left = tileCoords + FVector(-worldScaler, 0.f, 0.f);
	right = tileCoords + FVector(worldScaler, 0.f, 0.f);

	// iterate through all tiles in parent room
	for (auto tile : parentRoom->tiles)
	{
		if (tile == i_Tile)
		{
			continue;
		}

		// check if the tile is not wall
		if (tile->GetOwner()->FindComponentByClass<UWallComponent>())
		{
			continue;
		}
			
		// check if tile loc is any one of 4 coords
		FVector tileLoc = tile->GetOwner()->GetActorLocation();
		tileLoc.Z = 0.f;
		if (tileLoc == up)
		{
			i_Tile->adjecentTiles.Add(North, tile);
		}
		else if (tileLoc == down)
		{
			i_Tile->adjecentTiles.Add(South, tile);
		}
		else if (tileLoc == right)
		{
			i_Tile->adjecentTiles.Add(East, tile);
		}
		else if (tileLoc == left)
		{
			i_Tile->adjecentTiles.Add(West, tile);
		}
			
	}

	return i_Tile->adjecentTiles.Num();

}

UTileComponent* ATileBoardGenerator::SpawnTile(TEnumAsByte<ETileType> tileType, URoomComponent* parentRoom, FVector2D positionInRoom, FRotator tileRotation)
{
	if (!tileBPs.Contains(tileType))
		return nullptr;

	TSubclassOf<class AActor> tileBP = tileBPs[tileType];
	if (!tileBP)
		return nullptr;

	// Get rot and pos
	FRotator tileRot = FRotator::ZeroRotator;
	FVector tilePos = FVector(worldScaler * positionInRoom.X, worldScaler * positionInRoom.Y, 0.f);

	// get actor and the component
	AActor* tileActor = GetWorld()->SpawnActor(tileBP.Get(), &tilePos, &tileRot);
	UTileComponent* tile = tileActor->FindComponentByClass<UTileComponent>();
	UDoorComponent* door = tileActor->FindComponentByClass<UDoorComponent>();
	UWallComponent* wall = tileActor->FindComponentByClass<UWallComponent>();

	if (wall)
	{
		wall->parentTile = tile;
	}

	// if the door component was found, update the door component datum
	if (door)
	{
		door->parentTile = tile;
		float yaw = tileRotation.Yaw;

		if(UKismetMathLibrary::InRange_FloatFloat(yaw, -5.f, 5.f)) { door->doorDirection = EDoorOrientation::North; }
		else if (UKismetMathLibrary::InRange_FloatFloat(yaw, 85.f, 95.f)) { door->doorDirection = EDoorOrientation::East; }
		else if (UKismetMathLibrary::InRange_FloatFloat(yaw, 175.f, 185.f)) { door->doorDirection = EDoorOrientation::South; }
		else if (UKismetMathLibrary::InRange_FloatFloat(yaw, 265.f, 275.f)) { door->doorDirection = EDoorOrientation::West; }

		door->connectedDoor = nullptr;
		parentRoom->doors.Add(door);
		// UE_LOG(LogTemp, Warning, TEXT("Door detected and added"));
		
	}

	// attach to room
	tileActor->AttachToActor(parentRoom->GetOwner(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Tile"));
	tileActor->SetActorRotation(tileRotation);

	// setting up the tile data
	tile->tileType = tileType;
	tile->parentRoom = parentRoom;
	tile->roomLocation = positionInRoom;
	parentRoom->tiles.Add(tile);

	// setting up the parentRoom's startTile
	if (!parentRoom->startTile && tile->tileType == Floor)
		parentRoom->startTile = tile;

	return tile;
}

