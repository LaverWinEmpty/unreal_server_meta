#include "Generic/GenericGame.h"
#include "Generic/GenericController.h"
#include "Generic/GenericCharacter.h"
#include "Server/DatabaseManager.h"

AGenericGame::AGenericGame() {
    PlayerControllerClass = AGenericController::StaticClass();
    DefaultPawnClass      = AGenericCharacter::StaticClass();
}

void AGenericGame::BeginPlay() {
    if(UManager::IsServer(this)) {

    }
}
