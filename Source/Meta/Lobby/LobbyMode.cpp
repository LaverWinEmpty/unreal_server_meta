#include "Lobby/LobbyMode.h"
#include "Lobby/LobbyHandler.h"
#include "UI/LoginUI.h"
#include "UI/LobbyUI.h"
#include "Client/CustomizePreviewActor.h"
#include "Manager/Manager.h"

ALobbyMode::ALobbyMode() {
    DefaultPawnClass      = ACustomizePreviewActor::StaticClass();
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
