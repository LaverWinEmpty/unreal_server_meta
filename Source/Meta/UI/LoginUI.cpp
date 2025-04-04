// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LoginUI.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Server/AccountManager.h"

void ULoginUI::NativeConstruct() {
	InputPW->SetIsPassword(true);
}
