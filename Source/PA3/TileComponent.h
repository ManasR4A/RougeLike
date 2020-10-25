// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TileType.h"
#include "TileComponent.generated.h"

class URoomComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PA3_API UTileComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTileComponent();

	UPROPERTY(EditAnywhere)
		TEnumAsByte<ETileType> tileType;

	UPROPERTY(EditAnywhere)
		TArray<UTileComponent*> adjecentTiles;

	UPROPERTY(EditAnywhere)
		FVector2D roomLocation;

	UPROPERTY(EditAnywhere)
		URoomComponent* parentRoom;

	UPROPERTY(EditAnywhere)
		AActor* Visitor;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FVector GetWorldLocation(float WorldScaler);

	bool MakeVictoryTile(UMaterialInterface* i_victoryMaterial);
	bool MakeLavaTile(UMaterialInterface* i_LavaMat);
		
};
