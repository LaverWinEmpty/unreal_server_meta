// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyBeginer.h"
#include "Manager/Manager.h"
#include "Kismet/GameplayStatics.h"

void ALobbyBeginer::BeginPlay() {
	Super::BeginPlay();

	if (UManager::IsClient(this)) {
		FString URL = UManager::GetServerAddress() + _T("LobbyLevel");
		UE_LOG(LogTemp, Log, _T("%s"), *URL);
		GetWorld()->GetFirstPlayerController()->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
	}

	else UGameplayStatics::OpenLevel(this, _T("LobbyLevel")); // Server
}
