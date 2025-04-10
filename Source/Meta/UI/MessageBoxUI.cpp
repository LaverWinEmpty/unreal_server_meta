// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MessageBoxUI.h"
#include "Components/Button.h"
#include "Lobby/LobbyHandler.h"

void UMessageBoxUI::NativeConstruct() {
	OkButton->OnClicked.AddDynamic(this, &ThisClass::Hide);
}

void UMessageBoxUI::Show() {
	SetVisibility(ESlateVisibility::Visible);
}

void UMessageBoxUI::Hide() {
	SetVisibility(ESlateVisibility::Hidden);
}
