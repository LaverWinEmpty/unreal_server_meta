// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GenericGame.generated.h"

/*********************************************************************************************************************
 * @brief 서버 기본 게임 모드입니다.
 *********************************************************************************************************************/
UCLASS()
class META_API AGenericGame: public AGameMode
{
	GENERATED_BODY()

public:
	AGenericGame();
};
