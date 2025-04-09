#include "Server/ClientSessionManager.h"
#include "Engine/NetConnection.h"
#include "GameFramework/PlayerController.h"

bool UClientSessionManager::IsExist(const FString& ID) {
	return ClientSession.Find(ID) != nullptr;
}

TWeakObjectPtr<UNetConnection> UClientSessionManager::GetUserSocket(const FString& ID) {
	return ClientSession[ID];
}

FString UClientSessionManager::GetUserID(APlayerController* In) {
	TWeakObjectPtr<UNetConnection> Socket = In->GetNetConnection();
	return FString(ClientReference[Socket]);
}

void UClientSessionManager::OnLogIn(const APlayerController* In, const FString& ID) {
	check(UManager::IsServer(this));

	UNetConnection* Socket = In->GetNetConnection();
	if (Socket == nullptr) {
		if (!IsStandalone()) {
			checkf(false, _T("OnLonIn(): NOT STANDALONE MODE BUT NOT CONNECTED"));
		}
	}
	ClientSession.Add(ID, Socket);
	ClientReference.Add(Socket, ID);
}

void UClientSessionManager::OnLogOut(const APlayerController* In) {
	check(UManager::IsServer(this));

	UNetConnection* Socket = In->GetNetConnection();
	FString ID = ClientReference[Socket];
	ClientSession.Remove(ID);
	ClientReference.Remove(Socket);
}

void UClientSessionManager::OnLogOut(const FString& ID) {
	check(UManager::IsServer(this));
	
	TWeakObjectPtr<UNetConnection> Socket = ClientSession[ID];
	ClientSession.Remove(ID);
	ClientReference.Remove(Socket);
}
