// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LoginUI.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Server/AccountManager.h"

void ULoginUI::NativeConstruct() {
	InputPW->SetIsPassword(true);
}

// to server
void ULoginUI::OnLoginButtonClicked_Implementation(const FString& ID, const FString& PW) {
	UManager::Instance<UAccountManager>(this)->Authenticate(EAA_LogIn, ID, PW);
}

// from server
void ULoginUI::OnLoginFailed_Implementation(const FString& Result) {
	
}

// from server
void ULoginUI::OnLoginSucceded_Implementation() {
}
