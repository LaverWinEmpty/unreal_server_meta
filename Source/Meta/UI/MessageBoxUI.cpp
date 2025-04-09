// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MessageBoxUI.h"
#include "Components/Button.h"
#include "Lobby/LobbyHandler.h"

void UMessageBoxUI::Bind(ALobbyHandler* In) {
	OkButton->OnClicked.AddDynamic(In, &ALobbyHandler::HideMessageBox);
}
