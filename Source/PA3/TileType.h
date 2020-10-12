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
