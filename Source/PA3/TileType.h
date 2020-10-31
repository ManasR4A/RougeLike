// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "TileType.generated.h"

/**
 * 
 */
UENUM()
enum ETileType
{
	Floor, 
	Wall,
	Door,
	Victory,
	Lava,
	Upgrade,
	Empty
};

UENUM()
enum EDoorOrientation
{
	North,
	East,
	West,
	South
};

UENUM()
enum EWeapon
{
	Spear,
	None
};

UENUM()
enum EAction
{
	Jump,
	Attack,
	Push,
	Throw
};

UENUM()
enum EUpgrades
{
	FireProtection,
	ThrowRadiusIncrease,
	JumpRadiusIncrease,
	MeleaDamageIncrease,
	MaxHealthIncrease,
	MaxManaIncrease,
	HealthRefill,
	ManaRefill
};

UENUM()
enum EEnemyType
{
	Warrior,
	Archer
};

//UENUM()
//enum EPickupType
//{
//	Spear,
//	None
//};
