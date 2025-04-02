// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/LoginMode.h"
#include "Client/LoginController.h"

ALoginMode::ALoginMode() {
	DefaultPawnClass      = nullptr;
	PlayerControllerClass = ALoginController::StaticClass();
}
