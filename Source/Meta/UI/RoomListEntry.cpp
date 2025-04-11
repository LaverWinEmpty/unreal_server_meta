// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RoomListEntry.h"
#include "UI/RoomListEntryData.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Lobby/LobbyHandler.h"
#include "LobbyUI.h"

void URoomListEntry::NativeOnListItemObjectSet(UObject* In) {
	URoomListEntryData* Data = Cast<URoomListEntryData>(In);
	check(Data);
	Name->SetText(FText::FromString(Data->Name));
	Button->OnClicked.AddDynamic(this, &URoomListEntry::OnSelected);
}

void URoomListEntry::OnSelected() {
	ALobbyHandler* PC = Cast<ALobbyHandler>(GetOwningPlayer());
	if (!PC) {
		check(false);
		return;
	}
	
	// TODO: ClientTravel
}
