#include "Lobby/LobbyMode.h"
#include "Lobby/LobbyHandler.h"
#include "UI/LoginUI.h"
#include "UI/LobbyUI.h"
#include "Manager/Manager.h"

ALobbyMode::ALobbyMode() {
    DefaultPawnClass      = nullptr;
    PlayerControllerClass = ALobbyHandler::StaticClass();
}

void ALobbyMode::BeginPlay() {
    Super::BeginPlay();
    //if(UManager::IsServer()) {
    //    BeginServer();
    //} else BeginClient();
}
//
//void ALobbyMode::BeginClient() {
//    UE_LOG(LogTemp, Log, _T("ClientBegin: ClientTravel [%s]"), UManager::GetNetModeString(this));
//    ClientTravel(*Param, ETravelType::TRAVEL_Absolute);
//}
