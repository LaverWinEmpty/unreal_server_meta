// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InputBoxUI.h"
#include "Components/EditableText.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"

void UInputBoxUI::NativeConstruct() {
	OkButton->OnClicked.AddDynamic(this, &ThisClass::Hide);
}

void UInputBoxUI::Show() {
	SetVisibility(ESlateVisibility::Visible);
}

void UInputBoxUI::Hide() {
	SetVisibility(ESlateVisibility::Hidden);
}
