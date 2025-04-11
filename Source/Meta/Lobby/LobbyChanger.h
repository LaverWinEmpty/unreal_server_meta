// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyChanger.generated.h"

/**
 * 
 */
UCLASS()
class META_API ALobbyChanger : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void BeginPlay() override;
};
