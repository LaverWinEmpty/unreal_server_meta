// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/LobbyMode.h"
#include "Client/LobbyController.h"
#include "Client/CustomizePreviewActor.h"

ALobbyMode::ALobbyMode() {
	DefaultPawnClass      = ACustomizePreviewActor::StaticClass();
	PlayerControllerClass = ALobbyController::StaticClass();
}
