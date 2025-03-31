// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/GenericGame.h"
#include "Actor/GenericController.h"
#include "Actor/GenericCharacter.h"
#include "Server/DatabaseManager.h"
#include "Server/AccountManager.h"

AGenericGame::AGenericGame() {
	PlayerControllerClass = AGenericController::StaticClass();
	DefaultPawnClass      = AGenericCharacter::StaticClass();
}

void AGenericGame::BeginPlay() {
	if (UManager::IsServer(this)) {
		// UManager::Instance<UDatabaseManager>(this)->Setup();

		// 테스트
		UManager::Instance<UAccountManager>(this)->Authenticate(EAA_SignUp, "admin", "1q2w");
	}

}
