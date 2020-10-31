// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TileType.h"

#include "GameFramework/Actor.h"
#include "BasePickup.generated.h"

UCLASS()
class PA3_API ABasePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePickup();

	UFUNCTION(BlueprintCallable, Category = "Base Pickup")
		virtual void CollectPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Pickup")
		UStaticMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Pickup")
		TEnumAsByte<EWeapon> weaponType = None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Pickup")
		class UTileComponent* parentTile;

};
