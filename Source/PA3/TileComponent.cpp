// Fill out your copyright notice in the Description page of Project Settings.


#include "TileComponent.h"
#include "RoomComponent.h"
#include "VictoryComponent.h"

// Sets default values for this component's properties
UTileComponent::UTileComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTileComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FVector UTileComponent::GetWorldLocation(float WorldScaler)
{
	FVector WL = parentRoom->GetOwner()->GetActorLocation() + FVector(roomLocation * WorldScaler, 0.f);
	return WL;
}

bool UTileComponent::MakeVictoryTile(UMaterialInterface* i_victoryMaterial)
{
	UVictoryComponent* VictoryComponent = NewObject<UVictoryComponent>(GetOwner(), UVictoryComponent::StaticClass(), TEXT("VictoryComponent"));
	VictoryComponent->RegisterComponent();

	VictoryComponent->parentTile = this;
	this->tileType = Victory;
	auto mesh = this->GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	if (mesh)
	{
		mesh->SetMaterial(0, i_victoryMaterial);
	}

	return true;
}

bool UTileComponent::MakeLavaTile(UMaterialInterface* i_LavaMat)
{

	this->tileType = Lava;
	auto mesh = this->GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	if (mesh)
	{
		mesh->SetMaterial(0, i_LavaMat);
	}

	return true;
}

bool UTileComponent::MakeUpgradeTile(UMaterialInterface* i_UpgradeMat)
{
	this->tileType = Upgrade;
	auto mesh = this->GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	if (mesh)
	{
		FloorMat = mesh->GetMaterial(0);
		mesh->SetMaterial(0, i_UpgradeMat);
	}

	return true;
}

