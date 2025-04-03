// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyController.generated.h"

class ULobbyUI;
class UCharacterCustomizeUI;
class ACustomizePreviewActor;

UCLASS()
class META_API ALobbyController : public APlayerController
{
	GENERATED_BODY()

public:
	ALobbyController();

public:
	void BeginPlay() override;

public:
	UFUNCTION() void OnEmotionNext();
	UFUNCTION() void OnEmotionPrev();
	UFUNCTION() void OnUpperNext();
	UFUNCTION() void OnUpperPrev();
	UFUNCTION() void OnLowerNext();
	UFUNCTION() void OnLowerPrev();
	UFUNCTION() void OnShoesNext();
	UFUNCTION() void OnShoesPrev();
	UFUNCTION() void OnCustomBegin();
	UFUNCTION() void OnCustomEnd();
	UFUNCTION() void OnCustomCancel();
	UFUNCTION() void OnLogOut();

private:
	int EmotionIndex = 0;
	int UpperIndex   = 0;
	int LowerIndex   = 0;
	int ShoesIndex   = 0;

private:
	int EmotionCount = 0;
	int UpperCount   = 0;
	int LowerCount   = 0;
	int ShoesCount   = 0;

public:
	USkeletalMesh* GetSelectedEmotionMesh() const;
	USkeletalMesh* GetSelectedUpperMesh() const;
	USkeletalMesh* GetSelectedLowerMesh() const;
	USkeletalMesh* GetSelectedShoesMesh() const;

private:
	TSubclassOf<UUserWidget> LobbyUiWidgetClass;
	TSubclassOf<UUserWidget> CharacterCustomUiWidgetClass;

private:
	ULobbyUI*              LobbyUI;
	UCharacterCustomizeUI* CharacterCustomUI;

private:
	ACustomizePreviewActor* Actor;
	ACameraActor*           Viewer;
};
