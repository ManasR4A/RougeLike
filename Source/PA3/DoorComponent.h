// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileType.h"
#include "Components/ActorComponent.h"
#include "DoorComponent.generated.h"

class UTileComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PA3_API UDoorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDoorComponent();

	UPROPERTY(EditAnywhere)
		UTileComponent* parentTile;

	UPROPERTY(EditAnywhere)
		TEnumAsByte<EDoorOrientation> doorDirection;

	UPROPERTY(EditAnywhere)
		UDoorComponent* connectedDoor;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
