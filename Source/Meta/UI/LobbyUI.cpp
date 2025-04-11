#include "UI/LobbyUI.h"
#include "Components/Button.h"
#include "Lobby/LobbyHandler.h"

void ULobbyUI::Bind(ALobbyHandler* In) {
	LogOutButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnLogOut);
	StartButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnStart);
	NewCharacterButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnCustomBegin);
	NewRoomButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnShowUICreateRoom);
}
