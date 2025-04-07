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
	void NextOuifit(int OutfitIndex);
	void PrevOutfit(int OutfitIndex);
	void BodySelect(int, bool = false); //!< false: 의상 0으로 초기화

public:
	UFUNCTION() void OnBodyNext();
	UFUNCTION() void OnBodyPrev();
	UFUNCTION() void OnFaceNext();
	UFUNCTION() void OnFacePrev();
	UFUNCTION() void OnHairNext();
	UFUNCTION() void OnHairPrev();
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
	FPlayerMeshInfo Selected;        //!< 현재 고른 캐릭터 정보
	//int32           SelectIndex = 0; //!< 캐릭터 리스트 뷰에서 선택한 번호, 0인 경우 캐릭터 없음
	int32           SelectMax   = 0; //!< 캐릭터 선택 가능 수

private:
	// 각 Body의 Outfit별 아이템 가짓수
	int OutfitItemCount[EPB_BodyCount][EPO_OutfitCount] = { 0 };

private:
	TSubclassOf<UUserWidget> LobbyUiWidgetClass;
	TSubclassOf<UUserWidget> CharacterCustomUiWidgetClass;

private:
	ULobbyUI*              LobbyUI;
	UCharacterCustomizeUI* CharacterCustomUI;

private:
	ACustomizePreviewActor* Actor;
	ACameraActor*           Viewer;

public:
	void SetPreviewCharacter(int32);

public:
	void AddListView(const FString& Name, const FPlayerMeshInfo& MeshInfo);
};
