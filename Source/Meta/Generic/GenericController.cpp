#include "Generic/GenericController.h"

AGenericController::AGenericController() {
    PrimaryActorTick.bCanEverTick = true;
    bReplicates                   = true;
}

void AGenericController::BeginPlay() {
    Super::BeginPlay();
}

// 응답 받고 처리되는 함수입니다.
void AGenericController::OnLogOut(int8 Type, int8 Result) {
    
}