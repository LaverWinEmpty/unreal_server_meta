// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerListViewEntry.h"
#include "UI/PlayerListViewEntryData.h"
#include "Components/TextBlock.h"

void UPlayerListViewEntry::NativeOnListItemObjectSet(UObject* In) {
	UPlayerListViewEntryData* Data = Cast<UPlayerListViewEntryData>(In);
	check(Data);
	Text->SetText(FText::FromString(Data->Name));
}
