#include "Client/GameIntroHandler.h"
#include "Server/AccountManager.h"

// TODO: 추후 외부에서 세팅하는 것으로 변경
const FString AGameIntroHandler::ServerIP = TEXT("127.0.0.1:7777");

AGameIntroHandler::AGameIntroHandler() {}

void AGameIntroHandler::BeginPlay() {
    Super::BeginPlay();

    // 서버 연결 -> Client Mode
    FString URL = FString::Printf(TEXT("%s"), *ServerIP);
    GetWorld()->GetFirstPlayerController()->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
}