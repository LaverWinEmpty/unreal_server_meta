// Fill out your copyright notice in the Description page of Project Settings.

#include "Client/GameIntro.h"
#include "Client/LoginController.h"


AGameIntro::AGameIntro() {
    DefaultPawnClass      = nullptr;
    PlayerControllerClass = ALoginController::StaticClass();
}

//void AGameIntro::BeginPlay() {
//
//}