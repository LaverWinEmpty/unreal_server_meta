// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Manager/Manager.h"
#include "ActorManager.generated.h"

/**
 * 
 */
UCLASS()
class META_API UActorManager : public UManager {
	GENERATED_BODY()

public:
	static constexpr int CHANNEL_MAX = 10;
	
public:
	//TMap<int8, TSet<TObjectPtr<AActor>>> Channels;
	TSet<TObjectPtr<AActor>> Channel[CHANNEL_MAX];
};
