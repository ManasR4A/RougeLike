// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "TileComponent.h"
#include "RoomComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseEnemy::DamageEnemy(int32 i_DamageAmount)
{
	this->Health -= i_DamageAmount;
	if (this->Health <= 0)
	{
		// remove this enemy from the room's Enemies list
		if (this->currentTile->parentRoom->EnemiesInRoom.Contains(this))
		{
			this->currentTile->parentRoom->EnemiesInRoom.Remove(this);
		}

		this->currentTile->Visitor = nullptr;
		Destroy();
	}
}

void ABaseEnemy::Push(TEnumAsByte<EDoorOrientation> i_PushedInDir)
{
	if (this->currentTile->adjecentTiles.Contains(i_PushedInDir))
	{
		UTileComponent* nextTileInDir = this->currentTile->adjecentTiles[i_PushedInDir];
		
		// Damage if pushed in Wall
		if (nextTileInDir->tileType == Wall)
		{
			DamageEnemy(1);
			return;
		}

		// kill if in Lava
		else if (nextTileInDir->tileType == Lava)
		{
			MoveToTile(nextTileInDir, true);
			DamageEnemy(this->Health);
			return;
		}

		// recussively push the enemies behind
		if (nextTileInDir->Visitor)
		{
			ABaseEnemy* NextTileVisitor = Cast<ABaseEnemy>(nextTileInDir->Visitor);
			if (NextTileVisitor)
			{
				NextTileVisitor->Push(i_PushedInDir);
			}
		}

		MoveToTile(nextTileInDir, true);

	}
}

void ABaseEnemy::MoveToTile(UTileComponent* i_TargetTile, bool i_bTeleport)
{
	this->currentTile->Visitor = nullptr;
	
	// for location
	FVector oldLoc = this->GetActorLocation();
	FVector newLoc = i_TargetTile->GetOwner()->GetActorLocation();

	if (true)
	{
		this->SetActorLocation(newLoc);

		// for rotation
		oldLoc.Z = 0.f;
		newLoc.Z = 0.f;
		FRotator oldRot = this->GetActorRotation();
		FVector DifVector = newLoc - oldLoc;
		FRotator newRot = DifVector.Rotation();
		newRot.Roll = oldRot.Roll;
		newRot.Pitch = oldRot.Pitch;
		this->SetActorRotation(newRot);
	}
	else
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this->GetController(), newLoc);
	}

	i_TargetTile->Visitor = this;
	this->currentTile = i_TargetTile;
}

bool ABaseEnemy::MoveToPlayer(APA3Character* i_playerChar)
{
	return false;
}

bool ABaseEnemy::TakeTurn(APA3Character* i_playerChar)
{
	return false;
}

bool ABaseEnemy::CanAttackPlayer(APA3Character* i_playerChar)
{
	return false;
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

