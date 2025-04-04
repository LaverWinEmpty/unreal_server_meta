// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/LobbyController.h"
#include "Client/CustomizePreviewActor.h"
#include "Manager/PlayerMeshManager.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "UI/CharacterCustomizeUI.h"
#include "UI/LobbyUI.h"
#include "Components/Button.h"

ALobbyController::ALobbyController() {
	// Lobby UI
	static ConstructorHelpers::FClassFinder<UUserWidget> LobbyUiFinder(TEXT("/Game/Assets/UI/BP_LobbyUI"));
	check(LobbyUiFinder.Class);
	LobbyUiWidgetClass = LobbyUiFinder.Class;
	static ConstructorHelpers::FClassFinder<UUserWidget> CharaCustomUiFinder(TEXT("/Game/Assets/UI/BP_CharacterCustomizeUI"));
	check(CharaCustomUiFinder.Class);
	CharacterCustomUiWidgetClass = CharaCustomUiFinder.Class;
}

void ALobbyController::BeginPlay() {
	// 마우스 보이게
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;

	// TODO: DB에서 캐릭터 있으면 가져와서 세팅하기

	// UI 생성 후 바인드
	CharacterCustomUI = CreateWidget<UCharacterCustomizeUI>(this, CharacterCustomUiWidgetClass);
	CharacterCustomUI->AddToViewport();
	CharacterCustomUI->SetVisibility(ESlateVisibility::Hidden);

	CharacterCustomUI->EmotionNextButton->OnClicked.AddDynamic(this, &ALobbyController::OnEmotionNext);
	CharacterCustomUI->EmotionPrevButton->OnClicked.AddDynamic(this, &ALobbyController::OnEmotionPrev);
	CharacterCustomUI->UpperNextButton->OnClicked.AddDynamic(this, &ALobbyController::OnUpperNext);
	CharacterCustomUI->UpperPrevButton->OnClicked.AddDynamic(this, &ALobbyController::OnUpperPrev);
	CharacterCustomUI->LowerNextButton->OnClicked.AddDynamic(this, &ALobbyController::OnLowerNext);
	CharacterCustomUI->LowerPrevButton->OnClicked.AddDynamic(this, &ALobbyController::OnLowerPrev);
	CharacterCustomUI->ShoesNextButton->OnClicked.AddDynamic(this, &ALobbyController::OnShoesNext);
	CharacterCustomUI->ShoesPrevButton->OnClicked.AddDynamic(this, &ALobbyController::OnShoesPrev);

	CharacterCustomUI->EnterButton->OnClicked.AddDynamic(this, &ALobbyController::OnCustomEnd);
	CharacterCustomUI->ExitButton->OnClicked.AddDynamic(this, &ALobbyController::OnCustomCancel);
	
	LobbyUI = CreateWidget<ULobbyUI>(this, LobbyUiWidgetClass);
	LobbyUI->AddToViewport();
	
	LobbyUI->LogOutButton->OnClicked.AddDynamic(this, &ALobbyController::OnLogOut);
	LobbyUI->NewCharacterButton->OnClicked.AddDynamic(this, &ALobbyController::OnCustomBegin);

	// get
	Actor = Cast<ACustomizePreviewActor>(GetPawn());

	// 카메라 설정
	Viewer = GetWorld()->SpawnActor<ACameraActor>(FVector{ 0, 200, 90 }, FRotator{ 0, -90, 0 });
	SetViewTarget(Viewer);

	Viewer->GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Orthographic);
	Viewer->GetCameraComponent()->SetOrthoWidth(500.0f);

	// 에셋 개수 가져오기
	auto Manager = UPlayerMeshManager::Instance(this);
	for (int i = 0; i < EPB_BodyCount; ++i) {
		for (int j = 0; j < EPO_OutfitCount; ++j) {
			OutfitSelectMax[i][j] = Manager->Assets[i].Outfit[j].Num();
		}
	}
}

void ALobbyController::NextOuifit(int OutfitType) {
	++OutfitSelected[OutfitType];
	if (OutfitSelected[OutfitType] >= OutfitSelectMax[BodyType][OutfitType]) {
		OutfitSelected[OutfitType] = 0;
	}
	Actor->SetOutfitMesh(OutfitType, GetSelectedOutfitMesh(OutfitType));
}

void ALobbyController::PrevOutfit(int OutfitType) {
	--OutfitSelected[OutfitType];
	if (OutfitSelected[OutfitType] < 0) {
		OutfitSelected[OutfitType] = OutfitSelectMax[BodyType][OutfitType] - 1;
	}
	Actor->SetOutfitMesh(OutfitType, GetSelectedOutfitMesh(OutfitType));
}

void ALobbyController::BodySelect(int In) {
	BodyType = In;

	// 바디 메시 로드
	Actor->SetBodyMesh(GetSelectedBodyMesh());

	// 의상 바디에 맞춰 초기화
	for (int i = 0; i < EPO_OutfitCount; ++i) {
		OutfitSelected[i] = 0;
		Actor->SetOutfitMesh(OutfitSelected[i], GetSelectedOutfitMesh(OutfitSelected[i]));
	}

	// Idle 모션 재생
	Actor->PlayAnimation(
		UPlayerMeshManager::Instance(this)->Assets[BodyType].Anim[EPA_Idle]
	);
}

void ALobbyController::OnBodyNext() {
	if (++BodyType >= EPB_BodyCount) {
		BodyType = 0;
	}
	BodySelect(BodyType);
}

void ALobbyController::OnBodyPrev() {
	if (--BodyType < 0) {
		BodyType = EPB_BodyCount - 1;
	}
	BodySelect(BodyType);
}

void ALobbyController::OnEmotionNext() { NextOuifit(EPO_Face); }
void ALobbyController::OnEmotionPrev() { PrevOutfit(EPO_Face); }
void ALobbyController::OnUpperNext() { NextOuifit(EPO_Upper); }
void ALobbyController::OnUpperPrev() { PrevOutfit(EPO_Upper); }
void ALobbyController::OnLowerNext() { NextOuifit(EPO_Lower); }
void ALobbyController::OnLowerPrev() { PrevOutfit(EPO_Lower); }
void ALobbyController::OnShoesNext() { NextOuifit(EPO_Shoes); }
void ALobbyController::OnShoesPrev() { PrevOutfit(EPO_Shoes); }

void ALobbyController::OnCustomBegin() {
	CharacterCustomUI->SetVisibility(ESlateVisibility::Visible);
	LobbyUI->SetVisibility(ESlateVisibility::Hidden);

	auto Manager = UPlayerMeshManager::Instance(this);

	// get default body and set
	BodySelect(0);
}

void ALobbyController::OnCustomEnd() {
	LobbyUI->SetVisibility(ESlateVisibility::Visible);
	CharacterCustomUI->SetVisibility(ESlateVisibility::Hidden);

	// Get
}

void ALobbyController::OnCustomCancel() {
	LobbyUI->SetVisibility(ESlateVisibility::Visible);
	CharacterCustomUI->SetVisibility(ESlateVisibility::Hidden);

	// TODO: 기존 거 로딩
}

void ALobbyController::OnLogOut() {
	// TODO:
}

USkeletalMesh* ALobbyController::GetSelectedBodyMesh() const {
	return UPlayerMeshManager::Instance(this)->Assets[BodyType].Body;
}

USkeletalMesh* ALobbyController::GetSelectedOutfitMesh(int OutfitType) const {
	const auto& Arr = UPlayerMeshManager::Instance(this)->Assets[BodyType].Outfit[OutfitType];
	int         Idx = OutfitSelected[OutfitType];
	
	check(!Arr[Idx]);
	return Arr[Idx];
}