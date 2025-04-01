// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginUI.generated.h"

class UEditableTextBox;
class UButton;
class UTextBlock;

UCLASS()
class META_API ULoginUI : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

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

    UFUNCTION(Server, Reliable) void OnLoginButtonClicked(const FString& ID, const FString& PW);
    UFUNCTION(Client, Reliable) void OnLoginSucceded();
    UFUNCTION(Client, Reliable) void OnLoginFailed(const FString& Result);

private:
    void OnLoginButtonClicked_Implementation(const FString& ID, const FString& PW);
    void OnLoginFailed_Implementation(const FString& Result);
    void OnLoginSucceded_Implementation();
};
