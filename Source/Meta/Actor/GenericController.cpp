// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/GenericController.h"
#include "Server/AccountManager.h"

AGenericController::AGenericController() {
	// TODO: IP:Port 세팅합니다
}

void AGenericController::BeginPlay() {
	UE_LOG(LogTemp, Log, TEXT("GenericController Begin: Network Mode [%s]"), UManager::GetNetModeString(this));
	switch (UManager::GetNetMode(this)) {
		case ENetMode::NM_Client:
		case ENetMode::NM_Standalone:
			UE_LOG(LogTemp, Log, TEXT("Client에 Controller가 생성되었습니다."));
			break;

		case ENetMode::NM_DedicatedServer:
		case ENetMode::NM_ListenServer:
			UE_LOG(LogTemp, Log, TEXT("Server에 Controller가 생성되었습니다."));
			break;

		default: checkNoEntry();
	}

	// TODO: 클라면 경우 요청합니다.
	// TODO: host면 direct로 처리합니다.
}

void AGenericController::ConnectRequest_Implementation(const FString& IP, int32 Port) {
}

void AGenericController::InputResponse_Implementation() {
}
