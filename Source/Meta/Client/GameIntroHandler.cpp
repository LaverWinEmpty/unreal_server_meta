#include "Client/GameIntroHandler.h"
#include "Manager/Manager.h"
#include "Kismet/GameplayStatics.h"

AGameIntroHandler::AGameIntroHandler() {}

void AGameIntroHandler::BeginPlay() {
    Super::BeginPlay();
    
    // make 127.0.0.1:7777/LoginLevel
    FString Param = UManager::GetServerAddress() + _T("/LoginLevel");
    UE_LOG(LogTemp, Log, _T("Connect to %s"), *Param);

    if (UManager::IsServer(this)) {
        UGameplayStatics::OpenLevel(this, "LoginLevel", true); // change level
        UE_LOG(LogTemp, Log, _T("Call OpenLevel from [%s]"), UManager::GetNetModeString(this));
        return;
    }

    UE_LOG(LogTemp, Log, _T("Call ClientTravel from [%s]"), UManager::GetNetModeString(this));
    ClientTravel(*Param, ETravelType::TRAVEL_Absolute); // change level
}