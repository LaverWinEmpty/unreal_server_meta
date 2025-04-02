#include "Client/GameIntro.h"
#include "Client/GameIntroHandler.h"

AGameIntro::AGameIntro() {
    DefaultPawnClass      = nullptr;
    PlayerControllerClass = AGameIntroHandler::StaticClass();
}