// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/LobbyController.h"
#include "Client/CustomizePreviewActor.h"
#include "Manager/PlayerMeshManager.h"
#include "Camera/CameraActor.h"
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

	// 로드한 애셋 수 미리 가져옵니다.
	UPlayerMeshManager* Manager = UPlayerMeshManager::Instance(this);
	EmotionCount = Manager->EmotionAssets.Num();
	UpperCount   = Manager->UpperAssets.Num();
	LowerCount   = Manager->LowerAssets.Num();
	ShoesCount   = Manager->ShoesAssets.Num();

	// 카메라 설정
	Viewer = GetWorld()->SpawnActor<ACameraActor>(FVector{ 0, 200, 90 }, FRotator{ 0, -90, 0 });
	SetViewTarget(Viewer);
}

void ALobbyController::OnEmotionNext() {
	++EmotionIndex;
	if (EmotionIndex >= EmotionCount) {
		EmotionIndex = 0;
	}
	Actor->SetEmotionMesh(GetSelectedEmotionMesh());
}

void ALobbyController::OnEmotionPrev() {
	--EmotionIndex;
	if (EmotionIndex < 0) {
		EmotionIndex = EmotionCount - 1;
	}
	Actor->SetEmotionMesh(GetSelectedEmotionMesh());
}

void ALobbyController::OnUpperNext() {
	++UpperIndex;
	if (UpperIndex >= UpperCount) {
		UpperIndex = 0;
	}
	Actor->SetUpperMesh(GetSelectedUpperMesh());

}

void ALobbyController::OnUpperPrev() {
	--UpperIndex;
	if (UpperIndex < 0) {
		UpperIndex = UpperCount - 1;
	}
	Actor->SetUpperMesh(GetSelectedUpperMesh());
}

void ALobbyController::OnLowerNext() {
	++LowerIndex;
	if (LowerIndex >= LowerCount) {
		LowerIndex = 0;
	}
	Actor->SetLowerMesh(GetSelectedLowerMesh());
}

void ALobbyController::OnLowerPrev() {
	--LowerIndex;
	if (LowerIndex < 0) {
		LowerIndex = LowerCount - 1;
	}
	Actor->SetLowerMesh(GetSelectedLowerMesh());
}

void ALobbyController::OnShoesNext() {
	++ShoesIndex;
	if (ShoesIndex >= ShoesCount) {
		ShoesIndex = 0;
	}
	Actor->SetShoesMesh(GetSelectedShoesMesh());
}

void ALobbyController::OnShoesPrev() {
	--ShoesIndex;
	if (ShoesIndex < 0) {
		ShoesIndex = ShoesCount - 1;
	}
	Actor->SetShoesMesh(GetSelectedShoesMesh());
}

void ALobbyController::OnCustomBegin() {
	CharacterCustomUI->SetVisibility(ESlateVisibility::Visible);
	LobbyUI->SetVisibility(ESlateVisibility::Hidden);

	Actor->SetBodyMesh();
	Actor->SetEmotionMesh(GetSelectedEmotionMesh());
	Actor->SetUpperMesh(GetSelectedUpperMesh());
	Actor->SetLowerMesh(GetSelectedLowerMesh());
	Actor->SetShoesMesh(GetSelectedShoesMesh());
	Actor->Body->PlayAnimation(UPlayerMeshManager::Instance(this)->IdleAnimationAsset, true); // 애니메이션 재생
}

void ALobbyController::OnCustomEnd() {
	LobbyUI->SetVisibility(ESlateVisibility::Visible);
	CharacterCustomUI->SetVisibility(ESlateVisibility::Hidden);
}

void ALobbyController::OnCustomCancel() {
	LobbyUI->SetVisibility(ESlateVisibility::Visible);
	CharacterCustomUI->SetVisibility(ESlateVisibility::Hidden);
}

void ALobbyController::OnLogOut() {
}

USkeletalMesh* ALobbyController::GetSelectedEmotionMesh() const {
	return UPlayerMeshManager::Instance(this)->EmotionAssets[EmotionIndex];
}

USkeletalMesh* ALobbyController::GetSelectedUpperMesh() const {
	return UPlayerMeshManager::Instance(this)->UpperAssets[UpperIndex];
}

USkeletalMesh* ALobbyController::GetSelectedLowerMesh() const {
	return UPlayerMeshManager::Instance(this)->LowerAssets[LowerIndex];
}

USkeletalMesh* ALobbyController::GetSelectedShoesMesh() const {
	return UPlayerMeshManager::Instance(this)->ShoesAssets[ShoesIndex];
}
