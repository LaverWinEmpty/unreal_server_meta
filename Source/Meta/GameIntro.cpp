// Fill out your copyright notice in the Description page of Project Settings.


#include "GameIntro.h"
#include "UI/LoginUI.h"
#include "Components/Button.h"
#include "Server/AccountManager.h"

AGameIntro::AGameIntro() {
	DefaultPawnClass = nullptr;

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder(TEXT("/Game/Assets/UI/BP_LoginUI"));
	if (WidgetFinder.Succeeded()) {
		LoginWidgetClass = WidgetFinder.Class;
	}

}

void AGameIntro::BeginPlay() {
	ULoginUI* Widget = CreateWidget<ULoginUI>(GetWorld(), LoginWidgetClass);
	if (!Widget) {
		check(false);
	}

	if (UButton* Btn = Cast<UButton>(Widget->LoginButton)) {
		auto AccountMgr = UManager::Instance<UAccountManager>(this);
		Btn->OnClicked.AddDynamic(this, &AGameIntro::OnLogin);
	}
}
