// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobyUI.generated.h"

class UButton;
class UListView;

// TOOD: 오타수정

UCLASS()
class META_API ULobyUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UButton* LogOutButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CreateCharacterButton;

	//UPROPERTY(meta = (BindWidget))
	//UButton* CreateRoomButton;
	
	//UPROPERTY(meta = (BindWidget))
	//UButton* EnterRoomButton;

	//UPROPERTY(meta = (BindWidget))
	//UListView* Rooms;

	UFUNCTION()
	void OnCreateCharacter();
};
