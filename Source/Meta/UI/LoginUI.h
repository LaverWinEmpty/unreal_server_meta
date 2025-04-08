// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginUI.generated.h"

class UEditableTextBox;
class UButton;
class UTextBlock;

class ALobbyHandler;

UCLASS()
class META_API ULoginUI : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

public:
    void Bind(ALobbyHandler*);

public:
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* InputID;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* InputPW;

    UPROPERTY(meta = (BindWidget))
    UButton* SignUpButton;

    UPROPERTY(meta = (BindWidget))
    UButton* LogInButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ResultText;
};
