// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameIntro.generated.h"

class ULoginUI;
UCLASS()
class META_API AGameIntro : public AGameMode
{
	GENERATED_BODY()
	
public:
	AGameIntro();

public:
	void BeginPlay() override;

public:
	void OnLogin();

private:
	TSubclassOf<UUserWidget> LoginWidgetClass;
};
