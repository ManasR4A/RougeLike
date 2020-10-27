// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "ArcherEnemy.generated.h"

/**
 * 
 */
UCLASS()
class PA3_API AArcherEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:

	AArcherEnemy();

	bool MoveToPlayer(APA3Character* i_playerChar) override;

	bool CanAttackPlayer(APA3Character* i_playerChar) override;

	bool TakeTurn(APA3Character* i_playerChar) override;

	bool TryMoveInDir(TEnumAsByte<EDoorOrientation> i_dir);

};
