// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/LobbyController.h"
#include "Client/CustomizePreviewActor.h"
#include "Manager/PlayerMeshManager.h"
#include "Server/DatabaseManager.h"
#include "Server/ClientSessionManager.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "UI/CharacterCustomizeUI.h"
#include "UI/LobbyUI.h"
#include "UI/PlayerListViewEntryData.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ListView.h"

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

	CharacterCustomUI->FaceNextButton->OnClicked.AddDynamic(this, &ALobbyController::OnFaceNext);
	CharacterCustomUI->FacePrevButton->OnClicked.AddDynamic(this, &ALobbyController::OnFacePrev);
	CharacterCustomUI->HairNextButton->OnClicked.AddDynamic(this, &ALobbyController::OnHairNext);
	CharacterCustomUI->HairPrevButton->OnClicked.AddDynamic(this, &ALobbyController::OnHairPrev);
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

	// 캐릭터 정보 로딩하기
	FString ID = UClientSessionManager::Instance(this)->GetPlayerID(this);
	UDatabaseManager::Instance(this)->Query(
		"SELECT * FROM player_tbl WHERE owner_id = ?",
		[ID](sql::PreparedStatement* In) {
			In->setString(1, TCHAR_TO_UTF8(*ID));
		},
		[this](sql::ResultSet* In) {
			// TODO: 이거 비동기라 ResultSet 지워지면서 pure virtual 에러 뜨는 것 같음
			// 게임 스레드에서 생성할 게 아닌
			// 여기서 한 번에 생성 후에 
			// 안전하게 게임 스레드로 넘겨야 할 것 같음

			while (In->next()) {
				AsyncTask(ENamedThreads::GameThread,
					[this, In]() {
						// TODO: bug fix
						//UPlayerListViewEntryData* Item = NewObject<UPlayerListViewEntryData>(this);

						//Item->Name = FString(In->getString("nickname").c_str());
						//Item->BodyIndex = In->getInt("body_type");
						//Item->FaceIndex = In->getInt("face_type");
						//Item->HairIndex = In->getInt("hair_type");
						//Item->UpperIndex = In->getInt("upper_type");
						//Item->LowerIndex = In->getInt("lower_type");
						//Item->ShoesIndex = In->getInt("shoes_type");

						//LobbyUI->PlayerCharacterList->AddItem(Item);
					}
				);
			} // end while
		}
	); // end Query
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
		Actor->SetOutfitMesh(i, GetSelectedOutfitMesh(i));
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

void ALobbyController::OnFaceNext() { NextOuifit(EPO_Face); }
void ALobbyController::OnFacePrev() { PrevOutfit(EPO_Face); }
void ALobbyController::OnHairNext() { NextOuifit(EPO_Hair); }
void ALobbyController::OnHairPrev() { PrevOutfit(EPO_Hair); }
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
	// Regist DB
	FString Name = CharacterCustomUI->NameInputBox->GetText().ToString();
	FString ID   = UClientSessionManager::Instance(this)->GetPlayerID(this);
	TArray<int> IndexList = {
		BodyType,
		OutfitSelected[EPO_Face],
		OutfitSelected[EPO_Hair],
		OutfitSelected[EPO_Upper],
		OutfitSelected[EPO_Lower],
		OutfitSelected[EPO_Shoes],
	};

	UDatabaseManager::Instance(this)->Query(
		"SELECT * FROM player_tbl WHERE nickname = ? LIMIT 1", // search 1
		[Name](sql::PreparedStatement* In) {
			In->setString(1, TCHAR_TO_UTF8(*Name));
		},
		[this, ID, Name, IndexList](sql::ResultSet* In) {
			if (In->next()) {
			// duplicated
				AsyncTask(ENamedThreads::GameThread,
					[]() {
						// TODO:
						UE_LOG(LogTemp, Warning, _T("Duplciated Nickname"));
					}
				);
			}
			else {
				UDatabaseManager::Instance(this)->Query(
					"INSERT INTO player_tbl VALUES(?, ?, ?, ?, ?, ?, ?, ?)",
					[ID, Name, IndexList](sql::PreparedStatement* In) {
						In->setString(1, TCHAR_TO_UTF8(*Name));
						In->setString(2, TCHAR_TO_UTF8(*ID));
						int Index = 3;
						for (auto Param : IndexList) {
							In->setInt(Index, Param);
							++Index;
						}
					}
				); // end query

				AsyncTask(ENamedThreads::GameThread,
					[this]() {
						LobbyUI->SetVisibility(ESlateVisibility::Visible);
						CharacterCustomUI->SetVisibility(ESlateVisibility::Hidden);
					} // end lambda
				); // end AsyncTask
			} // end else
		} // end process
	); // end query
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
	return Arr[Idx];
}