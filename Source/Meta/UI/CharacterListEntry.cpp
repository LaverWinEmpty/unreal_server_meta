// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterListEntry.h"
#include "UI/CharacterListEntryData.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Lobby/LobbyHandler.h"
#include "LobbyUI.h"

void UCharacterListEntry::NativeOnListItemObjectSet(UObject* In) {
	UCharacterListEntryData* Data = Cast<UCharacterListEntryData>(In);
	check(Data);
	Name->SetText(FText::FromString(Data->Name));
	Button->OnClicked.AddDynamic(this, &UCharacterListEntry::OnSelected);
	Index = Data->Index;
}

void UCharacterListEntry::OnSelected() {
	ALobbyHandler* PC = Cast<ALobbyHandler>(GetOwningPlayer());
	if (!PC) {
		check(false);
		return;
	}
	PC->SelectCharacterFromList(Index - 1); // order to index e.g. 1 -> 0
	// TODO: private로 바꾸고 함수로 변경하기
	PC->LobbyUI->PlayerNameText->SetText(Name->GetText());
}
