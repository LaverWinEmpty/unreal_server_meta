#include "Server/ClientSessionManager.h"
#include "Engine/NetConnection.h"
#include "GameFramework/PlayerController.h"

bool UClientSessionManager::IsExist(const FString& ID) {
	return ClientSession.Find(ID) != nullptr;
}

TWeakObjectPtr<UNetConnection> UClientSessionManager::GetPlayerSocket(FString& ID) {
	return ClientSession[ID];
}

FStringView UClientSessionManager::GetPlayerID(APlayerController* In) {
	TWeakObjectPtr<UNetConnection> Socket = In->GetNetConnection();
	return ClientReference[Socket];
}

void UClientSessionManager::OnLogIn(const APlayerController* In, const FString& ID) {
	check(UManager::IsServer(this));

	UNetConnection* Socket = In->GetNetConnection();
	ClientSession.Add(ID, Socket);
	ClientReference.Add(Socket, ID);
}

void UClientSessionManager::OnLogOut(const APlayerController* In) {
	check(UManager::IsServer(this));

	UNetConnection* Socket = In->GetNetConnection();
	FString ID = FString{ ClientReference[Socket] };
	ClientSession.Remove(ID);
	ClientReference.Remove(Socket);
}

void UClientSessionManager::OnLogOut(const FString& ID) {
	check(UManager::IsServer(this));
	
	TWeakObjectPtr<UNetConnection> Socket = ClientSession[ID];
	ClientSession.Remove(ID);
	ClientReference.Remove(Socket);
}
