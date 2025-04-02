// Fill out your copyright notice in the Description page of Project Settings.


#include "Generic/GenericController.h"
#include "Server/AccountManager.h"

AGenericController::AGenericController() {
    PrimaryActorTick.bCanEverTick = true;
    bReplicates                   = true;
}

void AGenericController::BeginPlay() {
    Super::BeginPlay();
}

// 응답 받고 처리되는 함수입니다.
void AGenericController::OnLogOut(int8 Type, int8 Result) {
    // UAccountManager::Instance(this)->Authenticate(EAA_LogOut ... ) 호출 후
    check(Type == EAA_LogOut); // 로그아웃만 허용

    // TODO:
}