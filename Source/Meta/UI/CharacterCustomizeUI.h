// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterCustomizeUI.generated.h"

class UButton;
class UEditableText;
class ALobbyHandler;

UCLASS()
class META_API UCharacterCustomizeUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Bind(ALobbyHandler*);

public:
	UPROPERTY(meta = (BindWidget)) UButton* FaceNextButton;
	UPROPERTY(meta = (BindWidget)) UButton* FacePrevButton;
	UPROPERTY(meta = (BindWidget)) UButton* HairNextButton;
	UPROPERTY(meta = (BindWidget)) UButton* HairPrevButton;
	UPROPERTY(meta = (BindWidget)) UButton* UpperNextButton;
	UPROPERTY(meta = (BindWidget)) UButton* UpperPrevButton;
	UPROPERTY(meta = (BindWidget)) UButton* LowerNextButton;
	UPROPERTY(meta = (BindWidget)) UButton* LowerPrevButton;
	UPROPERTY(meta = (BindWidget)) UButton* ShoesNextButton;
	UPROPERTY(meta = (BindWidget)) UButton* ShoesPrevButton;
	UPROPERTY(meta = (BindWidget)) UButton* EnterButton;
	UPROPERTY(meta = (BindWidget)) UButton* ExitButton;

public:
	UPROPERTY(meta = (BindWidget)) UEditableText* NameInputBox;
};
