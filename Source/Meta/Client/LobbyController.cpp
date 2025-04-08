//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "Client/LobbyController.h"
//#include "Client/CustomizePreviewActor.h"
//#include "Manager/PlayerMeshManager.h"
//#include "Server/DatabaseManager.h"
//#include "Server/ClientSessionManager.h"
//#include "Camera/CameraActor.h"
//#include "Camera/CameraComponent.h"
//#include "UI/CharacterCustomizeUI.h"
//#include "UI/LobbyUI.h"
//#include "UI/PlayerListViewEntryData.h"
//#include "UI/PlayerListViewEntry.h"
//#include "Components/Button.h"
//#include "Components/EditableText.h"
//#include "Components/ListView.h"
//#include "Data/PlayerInfo.h"
//
//ALobbyController::ALobbyController() {
//	// Lobby UI
//	static ConstructorHelpers::FClassFinder<UUserWidget> LobbyUiFinder(TEXT("/Game/UI/BP_LobbyUI"));
//	check(LobbyUiFinder.Class);
//	LobbyUiWidgetClass = LobbyUiFinder.Class;
//	static ConstructorHelpers::FClassFinder<UUserWidget> CharaCustomUiFinder(TEXT("/Game/UI/BP_CharacterCustomizeUI"));
//	check(CharaCustomUiFinder.Class);
//	CharacterCustomUiWidgetClass = CharaCustomUiFinder.Class;
//}
//
//void ALobbyController::BeginPlay() {
//	Super::BeginPlay();
//	// 마우스 보이게
//	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
//
//	// UI 생성 후 바인드
//	CharacterCustomUI = CreateWidget<UCharacterCustomizeUI>(this, CharacterCustomUiWidgetClass);
//	CharacterCustomUI->AddToViewport();
//	CharacterCustomUI->SetVisibility(ESlateVisibility::Hidden);
//
//	LobbyUI = CreateWidget<ULobbyUI>(this, LobbyUiWidgetClass);
//	LobbyUI->AddToViewport();
//}
//
//void ALobbyController::NextOuifit(int OutfitIndex) {
//	
//	++Selected.OutfitIndex[OutfitIndex];
//	if (Selected.OutfitIndex[OutfitIndex] >= OutfitItemCount[Selected.BodyIndex][OutfitIndex]) {
//		Selected.OutfitIndex[OutfitIndex] = 0;
//	}
//	Actor->SetOutfitMesh(OutfitIndex, GetSelectedOutfitMesh(OutfitIndex));
//}
//
//void ALobbyController::PrevOutfit(int OutfitIndex) {
//	--Selected.OutfitIndex[OutfitIndex];
//	if (Selected.OutfitIndex[OutfitIndex] < 0) {
//		Selected.OutfitIndex[OutfitIndex] = OutfitItemCount[Selected.BodyIndex][OutfitIndex] - 1;
//	}
//	Actor->SetOutfitMesh(OutfitIndex, GetSelectedOutfitMesh(OutfitIndex));
//}
//
//void ALobbyController::BodySelect(int In, bool bPreInit) {
//	Selected.BodyIndex = In;
//
//	// 바디 메시 로드
//	Actor->SetBodyMesh(GetSelectedBodyMesh());
//
//	// 의상 바디에 맞춰 초기화
//	for (int i = 0; i < EPO_OutfitCount; ++i) {
//		Selected.OutfitIndex[i] *= static_cast<int>(bPreInit); // false: 의상 초기화
//		Actor->SetOutfitMesh(i, GetSelectedOutfitMesh(i));
//	}
//
//	// Idle 모션 재생
//	Actor->PlayAnimation(
//		UPlayerMeshManager::Instance(this)->Assets[Selected.BodyIndex].Anim[EPA_Idle]
//	);
//}
//
//void ALobbyController::OnBodyNext() {
//	if (++Selected.BodyIndex >= EPB_BodyCount) {
//		Selected.BodyIndex = 0;
//	}
//	BodySelect(Selected.BodyIndex);
//}
//
//void ALobbyController::OnBodyPrev() {
//	if (--Selected.BodyIndex < 0) {
//		Selected.BodyIndex = EPB_BodyCount - 1;
//	}
//	BodySelect(Selected.BodyIndex);
//}
//
//void ALobbyController::OnFaceNext()  { NextOuifit(EPO_Face); }
//void ALobbyController::OnFacePrev()  { PrevOutfit(EPO_Face); }
//void ALobbyController::OnHairNext()  { NextOuifit(EPO_Hair); }
//void ALobbyController::OnHairPrev()  { PrevOutfit(EPO_Hair); }
//void ALobbyController::OnUpperNext() { NextOuifit(EPO_Upper); }
//void ALobbyController::OnUpperPrev() { PrevOutfit(EPO_Upper); }
//void ALobbyController::OnLowerNext() { NextOuifit(EPO_Lower); }
//void ALobbyController::OnLowerPrev() { PrevOutfit(EPO_Lower); }
//void ALobbyController::OnShoesNext() { NextOuifit(EPO_Shoes); }
//void ALobbyController::OnShoesPrev() { PrevOutfit(EPO_Shoes); }
//
//void ALobbyController::OnCustomBegin() {
//	CharacterCustomUI->SetVisibility(ESlateVisibility::Visible);
//	LobbyUI->SetVisibility(ESlateVisibility::Hidden);
//
//	auto Manager = UPlayerMeshManager::Instance(this);
//
//	// get default body and set
//	BodySelect(0);
//}
//
//void ALobbyController::OnCustomEnd() {
//	// 캐릭터 추가
//	// 플레이어 정보를 가져옵니다.
//	FString ID = UClientSessionManager::Instance(this)->GetUserID(this);
//	FString Name = CharacterCustomUI->NameInputBox->GetText().ToString();
//
//	FPlayerMeshInfo Param = Selected;
//
//	UDatabaseManager::Instance(this)->Query(
//		"SELECT * FROM player_tbl WHERE nickname = ? LIMIT 1", // search 1
//		[Name](sql::PreparedStatement* In) {
//			In->setString(1, TCHAR_TO_UTF8(*Name));
//		},
//		[this, ID, Name, Param](sql::ResultSet* In) {
//			if (In->next()) {
//				// duplicated
//				AsyncTask(ENamedThreads::GameThread,
//					[]() {
//						// TODO:
//						UE_LOG(LogTemp, Warning, _T("Duplciated Nickname"));
//					}
//				);
//			}
//			else {
//				UDatabaseManager::Instance(this)->Query(
//					"INSERT INTO player_tbl ("
//						"nickname,"
//						"owner_id,"
//						"body_type,"
//						"face_type,"
//						"hair_type,"
//						"upper_type,"
//						"lower_type,"
//						"shoes_type"
//						") VALUES(?, ?, ?, ?, ?, ?, ?, ?"
//					"); ",
//					[ID, Name, Param](sql::PreparedStatement* In) {
//						In->setString(1, TCHAR_TO_UTF8(*Name));
//						In->setString(2, TCHAR_TO_UTF8(*ID));
//						int Index = 3;
//
//						In->setInt(Index++, Param.BodyIndex);
//						In->setInt(Index++, Param.OutfitIndex[EPO_Face]);
//						In->setInt(Index++, Param.OutfitIndex[EPO_Hair]);
//						In->setInt(Index++, Param.OutfitIndex[EPO_Upper]);
//						In->setInt(Index++, Param.OutfitIndex[EPO_Lower]);
//						In->setInt(Index,   Param.OutfitIndex[EPO_Shoes]); // 마지막이니까 굳이 ++ 안 하기
//					}
//				); // end query
//
//				AsyncTask(ENamedThreads::GameThread,
//					[this, Name, Param]() {
//						AddListView(Name, Param);
//						// 만든 캐릭터 정보로 로딩합니다.
//						SelectIndex = SelectMax - 1;
//
//						// 캐릭터 선택 창으로 UI 전환합니다.
//						LobbyUI->SetVisibility(ESlateVisibility::Visible);
//						CharacterCustomUI->SetVisibility(ESlateVisibility::Hidden);
//					} // end lambda
//				); // end AsyncTask
//			} // end else
//		} // end process
//	); // end query
//}
//
//void ALobbyController::OnCustomCancel() {
//	LobbyUI->SetVisibility(ESlateVisibility::Visible);
//	CharacterCustomUI->SetVisibility(ESlateVisibility::Hidden);
//
//	// 취소: 캐릭터가 존재하면 첫 캐릭터, 아니면 안 보임
//	SetPreviewCharacter(0);
//}
//
//void ALobbyController::OnStart() {
//	UE_LOG(LogTemp, Log, _T("Clicked"));
//}
//
//USkeletalMesh* ALobbyController::GetSelectedBodyMesh() const {
//	return UPlayerMeshManager::Instance(this)->Assets[Selected.BodyIndex].Body;
//}
//
//USkeletalMesh* ALobbyController::GetSelectedOutfitMesh(int OutfitIndex) const {
//	const auto& Arr = UPlayerMeshManager::Instance(this)->Assets[Selected.BodyIndex].Outfit[OutfitIndex];
//	int         Idx = Selected.OutfitIndex[OutfitIndex];
//	return Arr[Idx];
//}
//
//// 0 ~ Max - 1
//void ALobbyController::SetPreviewCharacter(int32 Index) {
//	SelectIndex = Index; // Save
//
//	// 범위 밖이면 nullptr로 세팅합니다.
//	if (Index >= SelectMax || Index < 0) {
//		Actor->SetBodyMesh(nullptr);
//		for (int i = 0; i < EPO_OutfitCount; ++i) {
//			Actor->SetOutfitMesh(i, nullptr);
//		}
//		Actor->PlayAnimation(nullptr);
//		return;
//	}
//
//	const TArray<UObject*>& Items = LobbyUI->PlayerCharacterList->GetListItems();
//	UPlayerListViewEntryData* Item = Cast<UPlayerListViewEntryData>(Items[Index]);
//
//	// load
//	Selected.OutfitIndex[EPO_Face]  = Item->FaceIndex;
//	Selected.OutfitIndex[EPO_Hair]  = Item->HairIndex;
//	Selected.OutfitIndex[EPO_Upper] = Item->UpperIndex;
//	Selected.OutfitIndex[EPO_Lower] = Item->LowerIndex;
//	Selected.OutfitIndex[EPO_Shoes] = Item->ShoesIndex;
//
//	// pre-init
//	BodySelect(Item->BodyIndex, true);
//}
//
//void ALobbyController::AddListView(const FString& Name, const FPlayerMeshInfo& MeshInfo) {
//	UPlayerListViewEntryData* Item = NewObject<UPlayerListViewEntryData>(this);
//	Item->Index      = ++SelectMax;
//	Item->Name       = Name;
//	Item->BodyIndex  = MeshInfo.BodyIndex;
//	Item->HairIndex  = MeshInfo.OutfitIndex[EPO_Hair];
//	Item->FaceIndex  = MeshInfo.OutfitIndex[EPO_Face];
//	Item->UpperIndex = MeshInfo.OutfitIndex[EPO_Upper];
//	Item->LowerIndex = MeshInfo.OutfitIndex[EPO_Lower];
//	Item->ShoesIndex = MeshInfo.OutfitIndex[EPO_Shoes];
//	LobbyUI->PlayerCharacterList->AddItem(Item);
//}