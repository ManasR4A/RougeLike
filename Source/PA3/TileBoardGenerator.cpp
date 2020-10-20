// Fill out your copyright notice in the Description page of Project Settings.
#include "TileBoardGenerator.h"
#include "RoomComponent.h"
#include "DoorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GenericPlatform/GenericPlatformMath.h"


// Sets default values
ATileBoardGenerator::ATileBoardGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
			UE_LOG(LogTemp, Error, TEXT("No Suitable Door Found in the random room"))
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

		i_DoorComponent->connectedDoor = doorToConnect;
		doorToConnect->connectedDoor = i_DoorComponent;
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
	
	// get doors from current room & iterate for each door
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
				UE_LOG(LogTemp, Warning, TEXT("All Door already connected."));
				continue;
			}
				
			UE_LOG(LogTemp, Warning, TEXT("Door already connected."));
			continue;
		}
			
		// Generate Room
		GenerateRoomAtDoor(door, VictoryRoom);
	
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
	}

	// Recursively Generate Room at the doors
	else if (currentRoom->roomDeapth < victoryDeapth - 1)
	{
		int count = 0;
		auto prevRoom = currentRoom;
		for (auto door : prevRoom->doors)
		{
			if (door->connectedDoor && currentRoom->roomDeapth == door->connectedDoor->parentTile->parentRoom->roomDeapth - 1)
			{
				count++;
				currentRoom = door->connectedDoor->parentTile->parentRoom;
				Generate();
				currentRoom = prevRoom;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Count = %d"), count);
	}

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
		UE_LOG(LogTemp, Warning, TEXT("Door detected and added"));
		
	}

	// attach to room
	tileActor->AttachToActor(parentRoom->GetOwner(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Tile"));
	tileActor->SetActorRotation(tileRotation);

	// setting up the tile data
	tile->tileType = tileType;
	tile->parentRoom = parentRoom;
	tile->roomLocation = positionInRoom;
	parentRoom->tiles.Add(tile);

	return tile;
}

