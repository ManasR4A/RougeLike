// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "WarriorEnemy.generated.h"

/**
 * 
 */

class UTileComponent;
class APA3Character;

UCLASS()
class PA3_API AWarriorEnemy : public ABaseEnemy
{
	GENERATED_BODY()
	
public:

	AWarriorEnemy();

	//void MoveToTile(UTileComponent* i_TargetTile, bool bTeleport) override;

	bool MoveToPlayer(APA3Character* i_playerChar) override;

	bool CanAttackPlayer(APA3Character* i_playerChar) override;
		
	bool TakeTurn(APA3Character* i_playerChar) override;

	bool TryMoveInDir(TEnumAsByte<EDoorOrientation> i_dir);
};
