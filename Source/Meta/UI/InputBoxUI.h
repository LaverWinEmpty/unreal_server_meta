// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputBoxUI.generated.h"

class UButton;
class UEditableTextBox;

UCLASS()
class META_API UInputBoxUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;

public:
	UFUNCTION()void Show();
	UFUNCTION()void Hide();

public:
	UPROPERTY(meta = (BindWidget)) UButton*          OkButton;
	UPROPERTY(meta = (BindWidget)) UButton*          CancelButton;
	UPROPERTY(meta = (BindWidget)) UEditableTextBox* InputBlock;
};