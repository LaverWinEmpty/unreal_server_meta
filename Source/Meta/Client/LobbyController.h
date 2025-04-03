#pragma once

#include "CoreMinimal.h"
#include "Enum/PlayerMeshEnum.h"
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
	void NextOuifit(int OutfitType);
	void PrevOutfit(int OutfitType);
	void BodySelect(int);

public:
	UFUNCTION() void OnBodyNext();
	UFUNCTION() void OnBodyPrev();
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

public:
	USkeletalMesh* GetSelectedOutfitMesh(int EOP) const;

private:
	int BodyType = 0;
	int Index[EPO_OutfitCount];
	int IndexMax[EPB_BodyCount][EPO_OutfitCount] = { 0 };

//public:
//	USkeletalMesh* GetSelectedEmotionMesh() const;
//	USkeletalMesh* GetSelectedUpperMesh() const;
//	USkeletalMesh* GetSelectedLowerMesh() const;
//	USkeletalMesh* GetSelectedShoesMesh() const;

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
