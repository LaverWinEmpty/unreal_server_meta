// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Manager/Manager.h"
#include "RootManager.generated.h"

/*********************************************************************************************************************
 * All Managers Management Class
 *********************************************************************************************************************/
UCLASS()
class META_API URootManager: public UManager {
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection);
};