// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUI.generated.h"

class UButton;
class UTextBlock;
class UListView;
class ALobbyHandler;
class UImage;

UCLASS()
class META_API ULobbyUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Bind(ALobbyHandler*);

public:
	UPROPERTY(meta = (BindWidget)) UButton* LogOutButton;
	UPROPERTY(meta = (BindWidget)) UButton* NewCharacterButton;

	UPROPERTY(meta = (BindWidget)) UButton* NewRoomButton;
	UPROPERTY(meta = (BindWidget)) UButton* StartButton;

	UPROPERTY(meta = (BindWidget)) UTextBlock* PlayerNameText;
	UPROPERTY(meta = (BindWidget)) UListView* PlayerCharacterList;
	UPROPERTY(meta = (BindWidget)) UListView* RoomList;

	// unused
	UPROPERTY(meta = (BindWidget))
	UImage* PreviewImage;
};
