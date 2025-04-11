// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyChanger.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/Manager.h"

void ALobbyChanger::BeginPlay() {
	Super::BeginPlay();

	// To Listen Server
	UGameplayStatics::OpenLevel(this, _T("TestLevel"), true, "listen");
}
