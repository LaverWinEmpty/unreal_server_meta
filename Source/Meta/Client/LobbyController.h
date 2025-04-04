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
	USkeletalMesh* GetSelectedBodyMesh() const;
	USkeletalMesh* GetSelectedOutfitMesh(int EPO) const;

private:
	int BodyType = 0;
	int OutfitSelected[EPO_OutfitCount]; // selected outfit index
	int OutfitSelectMax[EPB_BodyCount][EPO_OutfitCount] = { 0 };

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
