// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

class UTileBoardComponent;
class UTileComponent;
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Generation")
		bool bTileBoardGenerated;

	UPROPERTY(VisibleAnywhere, Category = "Generation")
		bool bTilesAddressed;

	UPROPERTY(VisibleAnywhere, Category = "Generation")
		bool bGenerationComplete;

	UPROPERTY(VisibleAnywhere)
		float WorldScaler = 200.f;

	UPROPERTY(VisibleAnywhere)
		UTileBoardComponent* tileBoard;

	UPROPERTY(VisibleAnywhere)
		APA3Character* playerCharecterRef;

};
