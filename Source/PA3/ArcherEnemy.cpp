// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherEnemy.h"


AArcherEnemy::AArcherEnemy()
{
    this->Health = 1;
    this->EnemyType = Archer;
}

bool AArcherEnemy::MoveToPlayer(APA3Character* i_playerChar)
{
    return false;
}

bool AArcherEnemy::CanAttackPlayer(APA3Character* i_playerChar)
{
    return false;
}

bool AArcherEnemy::TakeTurn(APA3Character* i_playerChar)
{
    return false;
}

bool AArcherEnemy::TryMoveInDir(TEnumAsByte<EDoorOrientation> i_dir)
{
    return false;
}
