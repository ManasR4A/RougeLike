// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileType.h"
#include "TileBoardGenerator.generated.h"

class UTileBoardComponent;
class UDoorComponent;
class URoomComponent;
class UTileComponent;
class UWallComponent;
class AGameManager;

UCLASS()
class PA3_API ATileBoardGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileBoardGenerator();

	UPROPERTY(EditAnywhere, Category = "Tile Board Generator|Generation")
		int32 seed;

	UPROPERTY(EditAnywhere, Category = "Tile Board Generator|Generation")
		int32 victoryDeapth;

	UPROPERTY(EditAnywhere, Category = "Tile Board Generator|Generation")
		int32 maxTries;

	UPROPERTY(EditAnywhere, Category = "Tile Board Generator|Tile Board Data")
		float worldScaler;

	UPROPERTY(EditAnywhere, Category = "Tile Board Generator|Tile Board Data")
		UTileBoardComponent* tileBoard;

	UPROPERTY(EditAnywhere, Category = "Tile Board Generator|Room Data")
		TArray<FString> roomFileNames;

	UPROPERTY(EditAnywhere, Category = "Tile Board Generator|Room Data")
		TArray<FString> victoryRoomNames;

	UPROPERTY(EditAnywhere, Category = "Tile Board Generator|Room Data")
		int StartingRoomFromArray;

	UPROPERTY(EditAnywhere, Category = "Tile Board Generator|Blueprint Refs")
		AGameManager* gameManagerRef;

	UPROPERTY(EditAnywhere, Category = "Tile Board Generator|Blueprint Refs")
		TSubclassOf<class AActor> roomBP;

	UPROPERTY(EditAnywhere, Category = "Tile Board Generator|Blueprint Refs")
		TSubclassOf<class AActor> boardBP;

	UPROPERTY(EditAnywhere, Category = "Tile Board Generator|Blueprint Refs")
		TMap<TEnumAsByte<ETileType>, TSubclassOf<AActor>> tileBPs;

	UPROPERTY(EditAnywhere, Category = "Tile Board Generator|Material Refs")
		UMaterialInterface* VictoryMat;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region Private Vars
	int32 tries = 0;
	bool FirstRoom = true;
	bool VictoryRoom = false;
	int32 currentDepth = 0;

	// used for generation of rooms and then poinint to the rooms for tile addressing
	URoomComponent* currentRoom;

	int32 m_totalGeneratedRooms = 0;
	int32 m_currentRoomIndex = 0;

#pragma endregion



	TEnumAsByte<EDoorOrientation> GetOppositeSide(TEnumAsByte<EDoorOrientation> i_doorDir);
	int32 hasOppositeDoorDIr(FString i_roomData, TEnumAsByte<EDoorOrientation> i_doorDir);

	UDoorComponent* GetFacingDoor(URoomComponent* i_roomToLookIn, TEnumAsByte<EDoorOrientation> i_dirToLooksFor);

	void GenerateRoomAtDoor(UDoorComponent* i_DoorComponent, bool genVictoryRoom);
	bool CheckForRoomCollision(URoomComponent* i_tempRoom);
	void allignRoomsViaDoors(URoomComponent* i_StaticRoom, UDoorComponent* i_StaticDoor, URoomComponent* i_RoomToMove, UDoorComponent* i_DoorToAllign);
	bool DestroyRoom(URoomComponent* i_room);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void Generate();

	UFUNCTION(BlueprintCallable)
		FString LoadRoomString(FString RoomFileName);

	UFUNCTION(BlueprintCallable)
		UTileComponent* SpawnTile(TEnumAsByte<ETileType> tileType, URoomComponent* parentRoom, FVector2D positionInRoom, FRotator tileRotation);

	UFUNCTION(BlueprintCallable)
		URoomComponent* CreateRoomComponentFromString(FString roomName, FString roomData, FVector2D roomOriginBoardPosition);

	UFUNCTION(BlueprintCallable)
		int32 GetAdjesentTiles(UTileComponent* i_Tile);

};
