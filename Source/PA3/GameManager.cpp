// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"

#include "PA3Character.h"
#include "TileBoardComponent.h"
#include "TileComponent.h"
#include "RoomComponent.h"
#include "BaseEnemy.h"
#include "WarriorEnemy.h"
#include "ArcherEnemy.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bTileBoardGenerated = false;
	bTilesAddressed = false;
	bGenerationComplete = false;

	MaxMana = 5;
	MaxHealth = 3;
	MaxJumpRadius = 2;
	MinThrowRadius = 2;
	MaxThrowRadius = 5;

	UpgradeProbability.Add(FireProtection, 0.0f);
	UpgradeProbability.Add(ThrowRadiusIncrease, 0.0f);
	UpgradeProbability.Add(JumpRadiusIncrease, 0.0f);
	UpgradeProbability.Add(MeleaDamageIncrease, 0.0f);
	UpgradeProbability.Add(MaxHealthIncrease, 0.0f);
	UpgradeProbability.Add(MaxManaIncrease, 0.0f);
	UpgradeProbability.Add(HealthRefill, 0.0f);
	UpgradeProbability.Add(ManaRefill, 0.0f);

}

void AGameManager::PostTileBoardGeneration()
{
	currentRoom = tileBoard->rooms[0];
	currentDeapth = currentRoom->roomDeapth;

	playerCharecterRef->gameManagerRef = this;
	playerCharecterRef->currentRoom = tileBoard->rooms[0];
	playerCharecterRef->currentTile = currentRoom->startTile;
	playerCharecterRef->currentTile->Visitor = playerCharecterRef;
	playerCharecterRef->Health = MaxHealth;
	playerCharecterRef->Mana = MaxMana;
	playerCharecterRef->JumpRadius = MaxJumpRadius;
	playerCharecterRef->ThrowRaius = MinThrowRadius;

	playerCharecterRef->SetActorLocation(playerCharecterRef->currentTile->GetWorldLocation(WorldScaler));
}

void AGameManager::PlayerInNewRoom(URoomComponent* i_OldRoom, URoomComponent* i_NewRoom)
{
	playerCharecterRef->currentRoom = i_NewRoom;
	playerCharecterRef->Mana = MaxMana;
}

TArray<TEnumAsByte<EUpgrades>> AGameManager::GetRandomUpgrade()
{
	TArray<TEnumAsByte<EUpgrades>> ret;

	for (int32 i = 0; i <= 3; i++)
	{
		bool Added = false;
		float roll = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		int32 rollint =  UKismetMathLibrary::FFloor(roll * 1000);
		UE_LOG(LogTemp, Warning, TEXT("upgrade roll = %d"), rollint);
		TArray<TEnumAsByte<EUpgrades>> keys;
		UpgradeProbability.GetKeys(keys);
		
		for (auto key : keys)
		//for (auto keyindex = keys.Num() - 1; keyindex >= 0 ; keyindex--)
		{
			//auto key = keys[keyindex];
			if (UpgradeProbability[key] > roll && !ret.Contains(key))
			{
				ret.AddUnique(key);
				Added = true;
				break;
			}
		}

		// to ensure we get 4 upgrades
		if (!Added)
			i--;
	}

	return ret;
}

bool AGameManager::ExecuteSelectedUpgrade(TEnumAsByte<EUpgrades> SelectedUpgrade)
{
	bool ret = false;

	switch (SelectedUpgrade)
	{
	case HealthRefill:
		playerCharecterRef->Health = MaxHealth;
		ret = true;
		break;

	case ManaRefill:
		playerCharecterRef->Mana = MaxMana;
		ret = true;
		break;

	case MaxHealthIncrease:
		MaxHealth++;
		playerCharecterRef->Health = MaxHealth;
		ret = true;
		break;

	case MaxManaIncrease:
		MaxMana++;
		playerCharecterRef->Mana = MaxMana;
		ret = true;
		break;

	case MeleaDamageIncrease:
		playerCharecterRef->MeleaDamage++;
		ret = true;
		break;

	case ThrowRadiusIncrease:
		if (playerCharecterRef->ThrowRaius < MaxThrowRadius)
		{
			playerCharecterRef->ThrowRaius++;
			ret = true;
		}
		break;

	case JumpRadiusIncrease:
		MaxJumpRadius++;
		if (playerCharecterRef->JumpRadius < MaxJumpRadius)
		{
			playerCharecterRef->JumpRadius++;
			ret = true;
		}
		break;
			
	case FireProtection:
		if (!playerCharecterRef->FireProtection)
		{
			playerCharecterRef->FireProtection = true;
			ret = true;
		}
		break;

	default:
		UE_LOG(LogTemp, Error, TEXT("Invalid Enum Upgrade given to ExecuteSelectedUpgrade()."));

	}

	// make the upgradde tile back to a normal floor tile.
	bUpgrade = false;
	playerCharecterRef->currentTile->tileType = Floor;
	auto mesh = playerCharecterRef->currentTile->GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	if (mesh)
	{
		auto floorMat = playerCharecterRef->currentTile->FloorMat;
		mesh->SetMaterial(0, floorMat);
	}

	return ret;
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	currentRoom = playerCharecterRef->currentRoom;

	switch (playerCharecterRef->currentTile->tileType)
	{
	case Victory:
		bVictory = true;
		//UKismetSystemLibrary::QuitGame(GetWorld(), Cast<APlayerController>(playerCharecterRef->GetController()), EQuitPreference::Quit, true);
		break;

	case Upgrade:
		bUpgrade = true;
		break;

	};

	if (playerCharecterRef->Health <= 0)
	{
		bFailure = true;
	}

	// if the player's turn is over, all the enemies in the room should take turn.
	if (!bPlayersTurn)
	{
		// look in currentRoom for the enemies and iterate over them.
		for (ABaseEnemy* Enemy : currentRoom->EnemiesInRoom)
		{
			AWarriorEnemy* warrior = Cast<AWarriorEnemy>(Enemy);
			if (warrior)
			{
				warrior->TakeTurn(playerCharecterRef);
			}
				

			AArcherEnemy* archer = Cast<AArcherEnemy>(Enemy);
			if (archer)
				archer->TakeTurn(playerCharecterRef);
			
		}

		bPlayersTurn = true;
	}
}

