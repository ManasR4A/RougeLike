// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "TileComponent.h"
#include "TileBoardComponent.h"

#include "RoomComponent.generated.h"


class UDoorComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PA3_API URoomComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URoomComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
		FVector2D roomOrigin;

	UPROPERTY(EditAnywhere)
		int32 roomDeapth;

	UPROPERTY(EditAnywhere)
		TArray<UTileComponent*> tiles;

	UPROPERTY(EditAnywhere)
		UTileComponent* startTile;

	UPROPERTY(EditAnywhere)
		TArray<UDoorComponent*> doors;

	UPROPERTY(EditAnywhere)
		FString roomName;

	UPROPERTY(EditAnywhere)
		TArray<UTileBoardComponent*> boards;
		
};
