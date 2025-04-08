// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LoginUI.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Lobby/LobbyHandler.h"

void ULoginUI::NativeConstruct() {
	InputPW->SetIsPassword(true);
}

void ULoginUI::Bind(ALobbyHandler* In) {
	LogInButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnLogIn);
	SignUpButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnSignUp);
}
