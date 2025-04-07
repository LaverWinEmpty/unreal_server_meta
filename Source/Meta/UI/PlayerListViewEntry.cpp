// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerListViewEntry.h"
#include "UI/PlayerListViewEntryData.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Client/LobbyController.h"

void UPlayerListViewEntry::NativeOnListItemObjectSet(UObject* In) {
	UPlayerListViewEntryData* Data = Cast<UPlayerListViewEntryData>(In);
	check(Data);
	Name->SetText(FText::FromString(Data->Name));
	Button->OnClicked.AddDynamic(this, &UPlayerListViewEntry::OnSelected);
	Index = Data->Index;
}

void UPlayerListViewEntry::OnSelected() {
	ALobbyController* PC = Cast<ALobbyController>(GetOwningPlayer());
	if (!PC) {
		check(false);
		return;
	}
	PC->SetPreviewCharacter(Index - 1); // order to index e.g. 1 -> 0
}
