// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyBeginer.generated.h"

/**
 * 
 */
UCLASS()
class META_API ALobbyBeginer : public AGameModeBase
{
	GENERATED_BODY()
	
	void BeginPlay() override;

};
