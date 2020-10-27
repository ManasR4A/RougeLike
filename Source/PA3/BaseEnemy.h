// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Tiletype.h"

#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

class UTileComponent;

UCLASS()
class PA3_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemy();

	UFUNCTION(BlueprintCallable, Category = "EnemyBase")
	virtual void DamageEnemy(int32 i_DamageAmount);
	
	UFUNCTION(BlueprintCallable, Category = "EnemyBase")
	virtual void Push(TEnumAsByte<EDoorOrientation> i_PushedInDir);

	UFUNCTION(BlueprintCallable, Category = "EnemyBase")
	virtual void MoveToTile(UTileComponent* i_TargetTile);

	UFUNCTION(BlueprintCallable, Category = "EnemyBase")
	virtual void MoveToPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyBase")
		int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyBase")
		UTileComponent* currentTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyBase")
		TEnumAsByte<EEnemyType> EnemyType;

};
