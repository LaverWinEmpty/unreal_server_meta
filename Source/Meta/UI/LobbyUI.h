// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUI.generated.h"

class UButton;

UCLASS()
class META_API ULobbyUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UButton* LogOutButton;

	UPROPERTY(meta = (BindWidget))
	UButton* NewCharacterButton;

	//UPROPERTY(meta = (BindWidget))
	//UButton* CreateRoomButton;

	//UPROPERTY(meta = (BindWidget))
	//UButton* EnterRoomButton;

	//UPROPERTY(meta = (BindWidget))
	//UListView* Rooms;

};
