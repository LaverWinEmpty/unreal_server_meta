// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterCustomizeUI.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class META_API UCharacterCustomizeUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget)) UTextBlock* EmotionText;
	UPROPERTY(meta = (BindWidget)) UButton*    EmotionNextButton;
	UPROPERTY(meta = (BindWidget)) UButton*    EmotionPrevButton;
};
