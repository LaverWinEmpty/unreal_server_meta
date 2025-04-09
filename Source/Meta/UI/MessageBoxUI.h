// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageBoxUI.generated.h"

class UButton;
class UTextBlock;
class ALobbyHandler;

UCLASS()
class META_API UMessageBoxUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void Bind(ALobbyHandler*);

public:
	UPROPERTY(meta = (BindWidget)) UButton*    OkButton;
	UPROPERTY(meta = (BindWidget)) UTextBlock* MessageBox;
};
