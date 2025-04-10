// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerListViewEntry.h"
#include "UI/PlayerListViewEntryData.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Lobby/LobbyHandler.h"
#include "LobbyUI.h"

void UPlayerListViewEntry::NativeOnListItemObjectSet(UObject* In) {
	UPlayerListViewEntryData* Data = Cast<UPlayerListViewEntryData>(In);
	check(Data);
	Name->SetText(FText::FromString(Data->Name));
	Button->OnClicked.AddDynamic(this, &UPlayerListViewEntry::OnSelected);
	Index = Data->Index;
}

void UPlayerListViewEntry::OnSelected() {
	ALobbyHandler* PC = Cast<ALobbyHandler>(GetOwningPlayer());
	if (!PC) {
		check(false);
		return;
	}
	PC->SelectCharacterFromList(Index - 1); // order to index e.g. 1 -> 0
	// TODO: private로 바꾸고 함수로 변경하기
	PC->LobbyUI->PlayerNameText->SetText(Name->GetText());
}
