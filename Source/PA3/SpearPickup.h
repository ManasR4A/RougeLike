// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePickup.h"
#include "SpearPickup.generated.h"

/**
 * 
 */
UCLASS()
class PA3_API ASpearPickup : public ABasePickup
{
	GENERATED_BODY()

public:
	ASpearPickup();

	void CollectPickup() override;
	
};
