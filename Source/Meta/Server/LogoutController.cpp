// Fill out your copyright notice in the Description page of Project Settings.


#include "Server/LogoutController.h"
#include "Server/ClientSessionManager.h"

ALogoutController::ALogoutController() {
}

void ALogoutController::BeginPlay() {
	Super::BeginPlay();
}

//void ALogoutController::LogoutRequest_Implementation() {
//	if (UManager::IsUser(this)) {
//		UE_LOG(LogTemp, Error, _T("로그아웃 요청이 Client에서 처리되었습니다. Mode: [%s]"), UManager::GetNetModeString(this));
//		return;
//	}
//	UClientSessionManager::Instance(this)->OnLogOut(this);
//}
//
//void ALogoutController::LogoutResponse_Implementation() {
//	if (UManager::IsServer(this)) {
//		UE_LOG(LogTemp, Error, _T("로그아웃 요청이 Server에서 처리되었습니다. Mode: [%s]"), UManager::GetNetModeString(this));
//	}
//	ClientTravel(_T("LoginLevel"), ETravelType::TRAVEL_Absolute);
//}
