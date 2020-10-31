// Fill out your copyright notice in the Description page of Project Settings.

#include "SpearPickup.h"
#include "TileComponent.h"
#include "PA3Character.h"

#include "Components/BoxComponent.h"

ASpearPickup::ASpearPickup()
{
	weaponType = Spear;
}

void ASpearPickup::CollectPickup()
{
	APA3Character* playerChar = Cast<APA3Character>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	playerChar->EquipWeapon(this);

	this->parentTile->Pickup = nullptr;
	Destroy();
}
