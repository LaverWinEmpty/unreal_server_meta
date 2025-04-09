#include "Server/ClientSessionManager.h"
#include "Engine/NetConnection.h"
#include "GameFramework/PlayerController.h"

void UClientSessionManager::Enter(const APlayerController* PC, TSharedPtr<FClientSession> CS) {
	check(UManager::IsServer(this));
	UNetConnection* Socket = PC->GetNetConnection();
	if (Socket == nullptr) {
		if (!IsStandalone()) {
			checkf(false, _T("Enter(): NOT STANDALONE MODE BUT NOT CONNECTED"));
		}
	}

	ClientSessionList.Add(Socket, CS);
	ClientSocketList.Add(CS->ID, Socket);
}

void UClientSessionManager::Leave(const APlayerController* In) {
	TWeakObjectPtr<UNetConnection> Socket        = In->GetNetConnection();
	TSharedPtr<FClientSession>     ClientSession = ClientSessionList[Socket];

	ClientSocketList.Remove(ClientSession->ID);
	ClientSessionList.Remove(Socket);
}

TSharedPtr<FClientSession> UClientSessionManager::GetClientSession(APlayerController* PC) {
	return ClientSessionList[PC->GetNetConnection()];
}

TWeakObjectPtr<UNetConnection> UClientSessionManager::GetSokcet(const FString& ID) {
	return ClientSocketList[ID];
}
