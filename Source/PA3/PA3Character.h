// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "TileType.h"

#include "GameFramework/Character.h"
#include "PA3Character.generated.h"

class AGameManager;

UCLASS(Blueprintable)
class APA3Character : public ACharacter
{
	GENERATED_BODY()

public:
	APA3Character();

	virtual void BeginPlay() override;

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	bool EquipWeapon(class ABasePickup* i_pickup);

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	UFUNCTION(BlueprintCallable, Category = "Player")
		void DamagePlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|References")
		AGameManager* gameManagerRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Position Data")
		class UTileComponent* currentTile;

	UPROPERTY(EditAnywhere, Category = "Player|Position Data")
		class URoomComponent* currentRoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		int32 Mana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		int32 MeleaDamage;

	UPROPERTY(EditAnywhere, Category = "Player")
		int32 JumpRadius;

	UPROPERTY(EditAnywhere, Category = "Player")
		int32 ThrowRaius;

	UPROPERTY(EditAnywhere, Category = "Player")
		bool FireProtection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		TEnumAsByte<EWeapon> EquipedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		bool canTick = false;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;
};

