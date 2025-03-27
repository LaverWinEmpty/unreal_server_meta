// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/GenericGame.h"
#include "Actor/GenericController.h"
#include "Actor/GenericCharacter.h"

AGenericGame::AGenericGame() {
	PlayerControllerClass = AGenericController::StaticClass();
	DefaultPawnClass      = AGenericCharacter::StaticClass();
}