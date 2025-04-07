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
#include "UI/PlayerListViewEntry.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ListView.h"
#include "Data/PlayerInfo.h"

ALobbyController::ALobbyController() {
	// Lobby UI
	static ConstructorHelpers::FClassFinder<UUserWidget> LobbyUiFinder(TEXT("/Game/UI/BP_LobbyUI"));
	check(LobbyUiFinder.Class);
	LobbyUiWidgetClass = LobbyUiFinder.Class;
	static ConstructorHelpers::FClassFinder<UUserWidget> CharaCustomUiFinder(TEXT("/Game/UI/BP_CharacterCustomizeUI"));
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
			OutfitItemCount[i][j] = Manager->Assets[i].Outfit[j].Num();
		}
	}

	// 리스트뷰 바인드하도록 세팅
	// this->LobbyUI->PlayerCharacterList->OnEntryWidgetGenerated().AddUObject(this, &ThisClass::OnEntryInit); */

	// 캐릭터 정보 로딩
	FString ID = UClientSessionManager::Instance(this)->GetPlayerID(this);
	UDatabaseManager::Instance(this)->Query(
		"SELECT * FROM player_tbl WHERE owner_id = ? ORDER BY created_at ASC",
		[ID](sql::PreparedStatement* In) {
			In->setString(1, TCHAR_TO_UTF8(*ID));
		},
		[this](sql::ResultSet* In) {
			TArray<FPlayerInfo> Params;
			while (In->next()) {
				FPlayerInfo Temp;
				Temp.Name = FString(In->getString("nickname").c_str());
				Temp.MeshInfo.BodyIndex = In->getInt("body_type");
				Temp.MeshInfo.OutfitIndex[EPO_Face] = In->getInt("face_type");
				Temp.MeshInfo.OutfitIndex[EPO_Hair] = In->getInt("hair_type");
				Temp.MeshInfo.OutfitIndex[EPO_Upper] = In->getInt("upper_type");
				Temp.MeshInfo.OutfitIndex[EPO_Lower] = In->getInt("lower_type");
				Temp.MeshInfo.OutfitIndex[EPO_Shoes] = In->getInt("shoes_type");
				Params.Add(Temp);
			}

			AsyncTask(ENamedThreads::GameThread,
				[this, In, Params]() {
					// 캐릭터 정보 없음
					if (Params.Num() == 0) {
						SetPreviewCharacter(-1); // set nullptr
						SelectMax = 0;
					}

					else {
						SelectMax = 0;
						for (auto& Param : Params) {
							AddListView(Param.Name, Param.MeshInfo);
						} // end for
					} // end else
				} // end lambda
			); // end async task
		} // end lambda
	); // end Query
}

void ALobbyController::NextOuifit(int OutfitIndex) {
	
	++Selected.OutfitIndex[OutfitIndex];
	if (Selected.OutfitIndex[OutfitIndex] >= OutfitItemCount[Selected.BodyIndex][OutfitIndex]) {
		Selected.OutfitIndex[OutfitIndex] = 0;
	}
	Actor->SetOutfitMesh(OutfitIndex, GetSelectedOutfitMesh(OutfitIndex));
}

void ALobbyController::PrevOutfit(int OutfitIndex) {
	--Selected.OutfitIndex[OutfitIndex];
	if (Selected.OutfitIndex[OutfitIndex] < 0) {
		Selected.OutfitIndex[OutfitIndex] = OutfitItemCount[Selected.BodyIndex][OutfitIndex] - 1;
	}
	Actor->SetOutfitMesh(OutfitIndex, GetSelectedOutfitMesh(OutfitIndex));
}

void ALobbyController::BodySelect(int In, bool bPreInit) {
	Selected.BodyIndex = In;

	// 바디 메시 로드
	Actor->SetBodyMesh(GetSelectedBodyMesh());

	// 의상 바디에 맞춰 초기화
	for (int i = 0; i < EPO_OutfitCount; ++i) {
		Selected.OutfitIndex[i] *= static_cast<int>(bPreInit); // false: 의상 초기화
		Actor->SetOutfitMesh(i, GetSelectedOutfitMesh(i));
	}

	// Idle 모션 재생
	Actor->PlayAnimation(
		UPlayerMeshManager::Instance(this)->Assets[Selected.BodyIndex].Anim[EPA_Idle]
	);
}

void ALobbyController::OnBodyNext() {
	if (++Selected.BodyIndex >= EPB_BodyCount) {
		Selected.BodyIndex = 0;
	}
	BodySelect(Selected.BodyIndex);
}

void ALobbyController::OnBodyPrev() {
	if (--Selected.BodyIndex < 0) {
		Selected.BodyIndex = EPB_BodyCount - 1;
	}
	BodySelect(Selected.BodyIndex);
}

void ALobbyController::OnFaceNext()  { NextOuifit(EPO_Face); }
void ALobbyController::OnFacePrev()  { PrevOutfit(EPO_Face); }
void ALobbyController::OnHairNext()  { NextOuifit(EPO_Hair); }
void ALobbyController::OnHairPrev()  { PrevOutfit(EPO_Hair); }
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
	// 캐릭터 추가
	// 플레이어 정보를 가져옵니다.
	FString ID = UClientSessionManager::Instance(this)->GetPlayerID(this);
	FString Name = CharacterCustomUI->NameInputBox->GetText().ToString();

	FPlayerMeshInfo Param;

	UDatabaseManager::Instance(this)->Query(
		"SELECT * FROM player_tbl WHERE nickname = ? LIMIT 1", // search 1
		[Name](sql::PreparedStatement* In) {
			In->setString(1, TCHAR_TO_UTF8(*Name));
		},
		[this, ID, Name, Param](sql::ResultSet* In) {
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
					"INSERT INTO player_tbl ("
						"nickname,"
						"owner_id,"
						"body_type,"
						"face_type,"
						"hair_type,"
						"upper_type,"
						"lower_type,"
						"shoes_type"
						") VALUES(?, ?, ?, ?, ?, ?, ?, ?"
					"); ",
					[ID, Name, Param](sql::PreparedStatement* In) {
						In->setString(1, TCHAR_TO_UTF8(*Name));
						In->setString(2, TCHAR_TO_UTF8(*ID));
						int Index = 3;

						In->setInt(Index++, Param.BodyIndex);
						In->setInt(Index++, Param.OutfitIndex[EPO_Face]);
						In->setInt(Index++, Param.OutfitIndex[EPO_Hair]);
						In->setInt(Index++, Param.OutfitIndex[EPO_Upper]);
						In->setInt(Index++, Param.OutfitIndex[EPO_Lower]);
						In->setInt(Index,   Param.OutfitIndex[EPO_Shoes]); // 마지막이니까 굳이 ++ 안 하기
					}
				); // end query

				AsyncTask(ENamedThreads::GameThread,
					[this, Name, Param]() {
						AddListView(Name, Param);
						// 캐릭터 선택 창으로 UI 전환합니다.
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

	// 취소: 캐릭터가 존재하면 첫 캐릭터, 아니면 안 보임
	SetPreviewCharacter(0);
}

void ALobbyController::OnLogOut() {
	// TODO:
}

USkeletalMesh* ALobbyController::GetSelectedBodyMesh() const {
	return UPlayerMeshManager::Instance(this)->Assets[Selected.BodyIndex].Body;
}

USkeletalMesh* ALobbyController::GetSelectedOutfitMesh(int OutfitIndex) const {
	const auto& Arr = UPlayerMeshManager::Instance(this)->Assets[Selected.BodyIndex].Outfit[OutfitIndex];
	int         Idx = Selected.OutfitIndex[OutfitIndex];
	return Arr[Idx];
}

// 0 ~ Max - 1
void ALobbyController::SetPreviewCharacter(int32 Index) {
	// 범위 밖이면 nullptr로 세팅합니다.
	if (Index >= SelectMax || Index < 0) {
		Actor->SetBodyMesh(nullptr);
		for (int i = 0; i < EPO_OutfitCount; ++i) {
			Actor->SetOutfitMesh(i, nullptr);
		}
		Actor->PlayAnimation(nullptr);
		return;
	}

	const TArray<UObject*>& Items = LobbyUI->PlayerCharacterList->GetListItems();
	UPlayerListViewEntryData* Item = Cast<UPlayerListViewEntryData>(Items[Index]);

	// load
	Selected.OutfitIndex[EPO_Face]  = Item->FaceIndex;
	Selected.OutfitIndex[EPO_Hair]  = Item->HairIndex;
	Selected.OutfitIndex[EPO_Upper] = Item->UpperIndex;
	Selected.OutfitIndex[EPO_Lower] = Item->LowerIndex;
	Selected.OutfitIndex[EPO_Shoes] = Item->ShoesIndex;

	// pre-init
	BodySelect(Item->BodyIndex, true);
}

void ALobbyController::AddListView(const FString& Name, const FPlayerMeshInfo& MeshInfo) {
	UPlayerListViewEntryData* Item = NewObject<UPlayerListViewEntryData>(this);
	Item->Index      = ++SelectMax;
	Item->Name       = Name;
	Item->BodyIndex  = MeshInfo.BodyIndex;
	Item->HairIndex  = MeshInfo.OutfitIndex[EPO_Hair];
	Item->FaceIndex  = MeshInfo.OutfitIndex[EPO_Face];
	Item->UpperIndex = MeshInfo.OutfitIndex[EPO_Upper];
	Item->LowerIndex = MeshInfo.OutfitIndex[EPO_Lower];
	Item->ShoesIndex = MeshInfo.OutfitIndex[EPO_Shoes];
	LobbyUI->PlayerCharacterList->AddItem(Item);
}