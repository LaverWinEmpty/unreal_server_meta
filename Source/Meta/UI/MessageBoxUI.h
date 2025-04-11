// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageBoxUI.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class META_API UMessageBoxUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;

public:
	UFUNCTION() void Show();
	UFUNCTION() void Hide();

public:
	UPROPERTY(meta = (BindWidget)) UButton*    OkButton;
	UPROPERTY(meta = (BindWidget)) UTextBlock* MessageBlock;
};
