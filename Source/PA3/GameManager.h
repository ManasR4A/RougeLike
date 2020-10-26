// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TileType.h"

#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

class UTileBoardComponent;
class UTileComponent;
class URoomComponent;
class APA3Character;

UCLASS()
class PA3_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

	/** To be called by TileBoardGenerater, after Generate() is complete*/
	void PostTileBoardGeneration();

	/** A helper function to run tasks when player moves to a new room. */
	void PlayerInNewRoom(URoomComponent* i_OldRoom, URoomComponent* i_NewRoom);

	/** Helper function to geta random upgrade. To be called by WBP_Upgrade */
	UFUNCTION(BlueprintCallable)
		TArray<TEnumAsByte<EUpgrades>> GetRandomUpgrade();

	/** Helper function to execute the selected upgrade. */
	UFUNCTION(BlueprintCallable)
		bool ExecuteSelectedUpgrade(TEnumAsByte<EUpgrades> SelectedUpgrade);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Game Manager|Generation")
		bool bTileBoardGenerated;

	UPROPERTY(VisibleAnywhere, Category = "Game Manager|Generation")
		bool bTilesAddressed;

	UPROPERTY(VisibleAnywhere, Category = "Game Manager|Generation")
		bool bGenerationComplete;

	UPROPERTY(VisibleAnywhere, Category = "Game Manager|Generation")
		float WorldScaler = 200.f;

	UPROPERTY(VisibleAnywhere, Category = "Game Manager|Generation")
		UTileBoardComponent* tileBoard;

	UPROPERTY(EditAnywhere, Category = "Game Manager|Gameplay")
		TMap<TEnumAsByte<EAction>, int32> ManaCosts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Manager|Gameplay")
		TMap<TEnumAsByte<EUpgrades>, float> UpgradeProbability;

	UPROPERTY(EditAnywhere, Category = "Game Manager|Gameplay")
		int32 MaxMana;
	
	UPROPERTY(EditAnywhere, Category = "Game Manager|Gameplay")
		int32 MaxHealth;
	
	UPROPERTY(EditAnywhere, Category = "Game Manager|Gameplay")
		int32 MaxJumpRadius;

	UPROPERTY(EditAnywhere, Category = "Game Manager|Gameplay")
		int32 MinThrowRadius;

	UPROPERTY(EditAnywhere, Category = "Game Manager|Gameplay")
		int32 MaxThrowRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Manager|Gameplay")
		bool bPlayersTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Manager|Gameplay")
		bool bVictory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Manager|Gameplay")
		bool bFailure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Manager|Gameplay")
		bool bUpgrade;

	UPROPERTY(VisibleAnywhere, Category = "Game Manager|Gameplay")
		APA3Character* playerCharecterRef;

	UPROPERTY(VisibleAnywhere, Category = "Game Manager|Gameplay")
		URoomComponent* currentRoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Manager|Gameplay|UI")
		int32 currentDeapth;

};
