// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LoginUI.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Server/AccountManager.h"

void ULoginUI::NativeConstruct() {
	// check: admin 등의 목적으로 사용 가능하나 현재는 클라이언트만 로그인 창이 뜨도록 처리했습니다
	if (UManager::IsServer(this)) {
		checkf(false, _T("Created log-in UI in Server, Mode: [%s]"), UManager::GetNetModeString(this));
	}
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
