// Fill out your copyright notice in the Description page of Project Settings.


#include "Server/LogoutController.h"
#include "Server/ClientSessionManager.h"

ALogoutController::ALogoutController() {
}

void ALogoutController::BeginPlay() {
	Super::BeginPlay();
}

void ALogoutController::OnLogOut() {
	// Dedicated, Listen, Standalone
	if (UManager::IsServer(this)) {
		LogoutRequest_Implementation(); // Direct call
	}
	// is Client
	else {
		LogoutRequest(); // Call
	}
}

void ALogoutController::LogoutRequest_Implementation() {
	if (UManager::IsUser(this)) {
		UE_LOG(LogTemp, Error, _T("로그아웃 요청이 Client에서 처리되었습니다. Mode: [%s]"), UManager::GetNetModeString(this));
		check(UManager::IsClient(this)); // client는 check
	}

	// remove from client session manager
	UClientSessionManager::Instance(this)->Leave(this);

	// make 127.0.0.1:7777/LoginLevel
	FString Param = UManager::GetServerAddress() + _T("/LoginLevel");

	// move
	ClientTravel(*Param, ETravelType::TRAVEL_Absolute); // change level

	// TODO: 뺄까
	PostLogout();
}

void ALogoutController::LogoutResponse_Implementation() {
	// TODO: 함수 자체를 지워도 될 거 같음 클라에서 할 게 없음

	if (UManager::IsServer(this)) {
		UE_LOG(LogTemp, Error, _T("로그아웃 요청이 Server에서 처리되었습니다. Mode: [%s]"), UManager::GetNetModeString(this));
		check(!UManager::IsDedicated(this)); // dedicated는 check
	}
	
	PostLogout();
}

void ALogoutController::PostLogout() { /* virtual */ }
