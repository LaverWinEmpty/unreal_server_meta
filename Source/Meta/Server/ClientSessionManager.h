// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Manager/Manager.h"
#include "ClientSessionManager.generated.h"

UCLASS()
class META_API UClientSessionManager : public UManager
{
	GENERATED_BODY()

public:


public:

	
private:
	TSet<FString> ClientSession;
};
