#include "LobbyHandler.h"

#include "UI/LoginUI.h"
#include "UI/LobbyUI.h"
#include "UI/CharacterCustomizeUI.h"
#include "UI/MessageBoxUI.h"
#include "UI/PlayerListViewEntryData.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/EditableTextBox.h"
#include "Components/ListView.h"

#include "Server/ClientSessionManager.h"
#include "Server/DatabaseManager.h"
#include "Manager/PlayerMeshManager.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"

#include "Enum/ResultEnum.h"

#include "Client/CustomizePreviewActor.h"

#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include "openssl/evp.h"
THIRD_PARTY_INCLUDES_END
#undef UI
#include "openssl/sha.h"

ALobbyHandler::ALobbyHandler() {
    bShowMouseCursor = true;

    // Login UI
    static ConstructorHelpers::FClassFinder<UUserWidget> LoginUiFinder(TEXT("/Game/UI/BP_LoginUI"));
    check(LoginUiFinder.Class);
    LoginUiClass = LoginUiFinder.Class;

    // Lobby UI
    static ConstructorHelpers::FClassFinder<UUserWidget> LobbyUiFinder(TEXT("/Game/UI/BP_LobbyUI"));
    check(LobbyUiFinder.Class);
    LobbyUiClass = LobbyUiFinder.Class;

    // Customize UI
    static ConstructorHelpers::FClassFinder<UUserWidget> CharaCustomUiFinder(TEXT("/Game/UI/BP_CharacterCustomizeUI"));
    check(CharaCustomUiFinder.Class);
    CharacterCustomizeUiClass = CharaCustomUiFinder.Class;

    // MessageBox UI
    static ConstructorHelpers::FClassFinder<UUserWidget> MessageBoxUiFinder(TEXT("/Game/UI/BP_MessageBoxUI"));
    check(MessageBoxUiFinder.Class);
    MessageBoxUiClass = MessageBoxUiFinder.Class;
}

void ALobbyHandler::BeginPlay() {
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, _T("Begin: [%s]"), UManager::GetNetModeString(this));
    // listen, standalone은 모두 호출됩니다.
    if(UManager::IsServer(this)) {
        BeginServer();
    } 
    if (UManager::IsUser(this)) {
        BeginClient();
    }
}

void ALobbyHandler::BeginClient() {
    // Login UI 생성 후 바인드
    LoginUI = CreateWidget<ULoginUI>(GetWorld(), LoginUiClass);
    LoginUI->AddToViewport();
    LoginUI->Bind(this);

    // Lobby UI 생성 후 바인드
    LobbyUI = CreateWidget<ULobbyUI>(GetWorld(), LobbyUiClass);
    LobbyUI->AddToViewport();
    LobbyUI->Bind(this);

    // Customize UI 생성 후 바인드
    CharacterCustomizeUI = CreateWidget<UCharacterCustomizeUI>(GetWorld(), CharacterCustomizeUiClass);
    CharacterCustomizeUI->AddToViewport();
    CharacterCustomizeUI->Bind(this);

    //MessageBoxUI = CreateWidget<UMessageBox>(GetWorld(), UMessageBoxUiClass);
    //MessageBoxUI->SetVisibility(ESlateVisibility::Hidden);
    //MessageBoxUI->AddToViewport();

    EnterLoginModeResponse(); // begin

    // get
    Actor = Cast<ACustomizePreviewActor>(GetPawn());

    // 에셋 개수 가져오기
    auto Manager = UPlayerMeshManager::Instance(this);
    for(int i = 0; i < EPB_BodyCount; ++i) {
        for(int j = 0; j < EPO_OutfitCount; ++j) {
            OutfitItemCount[i][j] = Manager->Assets[i].Outfit[j].Num();
        }
    }
}

void ALobbyHandler::BeginServer() {
    // camera
    Viewer = GetWorld()->SpawnActor<ACameraActor>(FVector{ 0, 200, 90 }, FRotator{ 0, -90, 0 });
    Viewer->GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Orthographic);
    Viewer->GetCameraComponent()->SetOrthoWidth(500.0f);
    SetViewTarget(Viewer);
}

void ALobbyHandler::EnterLobbyModeResponse() {
    UManager::Dispatch([this]() {
        RESPONSE(EnterLobbyModeToClient);
    });
}

void ALobbyHandler::EnterLoginModeResponse() {
    UManager::Dispatch([this]() {
        RESPONSE(EnterLoginModeToClient);
    });
}

void ALobbyHandler::EnterCustomizeModeResponse() {
    UManager::Dispatch([this]() {
        RESPONSE(EnterCustomizeModeToClient);
    });
}

void ALobbyHandler::PrintResultResponse(int8 Code) {
    UManager::Dispatch([this, Code]() {
        RESPONSE(PrintResultToClient, Code);
    });
}

void ALobbyHandler::NewCharacterResponse(const FString& Name, const FPlayerOutfit& Outfit) {
    UManager::Dispatch([this, Name, Outfit] {
        RESPONSE(NewCharacterToClient, Name, Outfit);
    });
}

void ALobbyHandler::LoadCharactersResponse(const TArray<FPlayerInfo>& Params) {
    UManager::Dispatch([this, Params] {
        RESPONSE(LoadCharactersToClient, Params);
    });
}

void ALobbyHandler::LogInToServer_Implementation(const FString& ID, const FString& PW) {
    check(UManager::IsServer(this));

    FString HashedPW = PasswordSHA256(PW);

    // Check account
    UDatabaseManager::Instance(this)->Query(
        "SELECT * FROM user_tbl WHERE user_id = ? and user_pw = ?",
        [=](sql::PreparedStatement* In) {
            In->setString(1, TCHAR_TO_UTF8(*ID));
            In->setString(2, TCHAR_TO_UTF8(*HashedPW));
        },
        [this, ID](sql::ResultSet* In) {
            if(In->next()) {
                LoadPlayerCharacterList(ID); // get list
                EnterLobbyModeResponse();    // move
            }
            PrintResultResponse(ERC_AlreadyExistID); // print message
        } // end lambda
    ); // end Query
}

void ALobbyHandler::LogOutToServer_Implementation(const FString& ID) {
    // TODO:
}

void ALobbyHandler::SignUpToServer_Implementation(const FString& ID, const FString& PW) {
    check(UManager::IsServer(this));

    FString HashedPW = PasswordSHA256(PW);

    EResultCode Result = ERC_Uknown;

    // Check account
    UDatabaseManager::Instance(this)->Query(
        "SELECT * FROM user_tbl WHERE user_id = ? and user_pw = ?",
        [=](sql::PreparedStatement* In) {
            In->setString(1, TCHAR_TO_UTF8(*ID));
            In->setString(2, TCHAR_TO_UTF8(*HashedPW));
        },
        // Manager is reference
        [this, ID, HashedPW](sql::ResultSet* In) {
            if(!In->next()) {
                UDatabaseManager::Instance(this)->Query(
                    "INSERT INTO user_tbl VALUES (?, ?)",
                    [=](sql::PreparedStatement* In) {
                        In->setString(1, TCHAR_TO_UTF8(*ID));
                        In->setString(2, TCHAR_TO_UTF8(*HashedPW));
                    },
                    [this](sql::ResultSet*) {
                        PrintResultToClient(ERC_Succeeded);
                    }
                ); // end Query
            } else PrintResultToClient(ERC_AlreadyExistID); // exist
        } // end lambda
    ); // end Query
}

void ALobbyHandler::SignOutToServer_Implementation(const FString& ID, const FString& PW) {
    // TODO:
}

void ALobbyHandler::NewCharacterToServer_Implementation(const FString& Name, const FPlayerOutfit& Outfit) {
    check(UManager::IsServer(this));

    // 플레이어 정보를 가져옵니다.
    FString ID = UClientSessionManager::Instance(this)->GetUserID(this);

    UDatabaseManager::Instance(this)->Query(
        "SELECT * FROM player_tbl WHERE nickname = ? LIMIT 1", // search 1
        [Name](sql::PreparedStatement* In) { In->setString(1, TCHAR_TO_UTF8(*Name)); },
        [this, ID, Name, Outfit](sql::ResultSet* In) {
            if (!In->next()) {
                FString Query = _T(
                    "INSERT INTO player_tbl ("
                        "nickname,"
                        "owner_id,"
                        "body_type,"
                        "face_type,"
                        "hair_type,"
                        "upper_type,"
                        "lower_type,"
                        "shoes_type"
                    ") VALUES (?, ?, ?, ?, ?, ?, ?, ?);"
                );

                UDatabaseManager::Instance(this)->Query(
                    Query,
                    [ID, Name, Outfit](sql::PreparedStatement* In) {
                        In->setString(1, TCHAR_TO_UTF8(*Name));
                        In->setString(2, TCHAR_TO_UTF8(*ID));
                        In->setInt(3, Outfit.BodyIndex);
                        In->setInt(4, Outfit.OutfitIndex[EPO_Face]);
                        In->setInt(5, Outfit.OutfitIndex[EPO_Hair]);
                        In->setInt(6, Outfit.OutfitIndex[EPO_Upper]);
                        In->setInt(7, Outfit.OutfitIndex[EPO_Lower]);
                        In->setInt(8, Outfit.OutfitIndex[EPO_Shoes]);
                    },
                    [this, Name, Outfit](sql::ResultSet*) {
                        PrintResultResponse(ERC_Succeeded);
                        NewCharacterResponse(Name, Outfit);
                    }
                );
            }
            else {
                PrintResultResponse(ERC_AlreadyExistName);
            }
        } // end process
    ); // end query
}

void ALobbyHandler::NewCharacterToClient_Implementation(const FString& Name, const FPlayerOutfit& Outfit) {
    check(UManager::IsUser(this));

    AddListView(Name, Outfit);
    // 만든 캐릭터 정보로 로딩합니다.
    SelectIndex = SelectMax - 1;
    // 캐릭터 선택 창으로 UI 전환합니다.
    EnterLobbyModeResponse();
}

void ALobbyHandler::LoadCharactersToClient_Implementation(const TArray<FPlayerInfo>& In) {
    // not exist
    if(In.Num() == 0) {
        SetPreviewCharacter(-1); // set nullptr
        SelectMax = 0;
    } else {
        SelectMax = 0;
        for(auto& Param : In) {
            AddListView(Param.Name, Param.MeshInfo);
        } // end for
    } // end else
}

void ALobbyHandler::EnterLoginModeToClient_Implementation() {
    LoginUI->SetVisibility(ESlateVisibility::Visible);
    LobbyUI->SetVisibility(ESlateVisibility::Hidden);
    CharacterCustomizeUI->SetVisibility(ESlateVisibility::Hidden);
}

void ALobbyHandler::EnterLobbyModeToClient_Implementation() {
    LoginUI->SetVisibility(ESlateVisibility::Hidden);
    LobbyUI->SetVisibility(ESlateVisibility::Visible);
    CharacterCustomizeUI->SetVisibility(ESlateVisibility::Hidden);
}

void ALobbyHandler::EnterCustomizeModeToClient_Implementation() {
    LoginUI->SetVisibility(ESlateVisibility::Hidden);
    LobbyUI->SetVisibility(ESlateVisibility::Hidden);
    CharacterCustomizeUI->SetVisibility(ESlateVisibility::Visible);
}

void ALobbyHandler::PrintResultToClient_Implementation(int8 Code) {
    // TODO: UI 에 메세지 출력
    UE_LOG(LogTemp, Log, _T("%s"), GetResultMessage(Code));
}

void ALobbyHandler::OnBodyNext() {
    if(++Selected.BodyIndex >= EPB_BodyCount) {
        Selected.BodyIndex = 0;
    }
    BodySelect(Selected.BodyIndex);
}

void ALobbyHandler::OnBodyPrev() {
    if(--Selected.BodyIndex < 0) {
        Selected.BodyIndex = EPB_BodyCount - 1;
    }
    BodySelect(Selected.BodyIndex);
}

void ALobbyHandler::OnCustomBegin() {
    EnterLobbyModeResponse();
    BodySelect(0); // init body
}

void ALobbyHandler::OnCustomEnd() {
    FString Name = CharacterCustomizeUI->NameInputBox->GetText().ToString();
    NewCharacterToServer(Name, Selected);
}

void ALobbyHandler::OnCustomCancel() {
    EnterLobbyModeResponse();
    SetPreviewCharacter(0); // 취소: 캐릭터가 존재하면 첫 캐릭터, 아니면 안 보임
}

void ALobbyHandler::OnStart() {
    UE_LOG(LogTemp, Log, _T("Clicked"));
}

FString ALobbyHandler::PasswordSHA256(const FString& In) {
    // salting and convert
    FString     PW          = GetSalted(In);
    std::string UTF8String  = TCHAR_TO_UTF8(*PW);
    const char* InputData   = UTF8String.c_str();
    size_t      InputLength = UTF8String.length();

    // buffer
    unsigned char Buffer[SHA256_DIGEST_LENGTH];

    // hasing
    SHA256_CTX Sha256Context;
    SHA256_Init(&Sha256Context);
    SHA256_Update(&Sha256Context, InputData, InputLength);
    SHA256_Final(Buffer, &Sha256Context);

    // convert to hex
    FString HashString;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        HashString += FString::Printf(TEXT("%02X"), Buffer[i]);
    }
    return HashString;
}

FString ALobbyHandler::GetSalted(const FString& In) {
    // 시늉만
    return In + _T("_Unreal_Meta_Project_Account_Manager_Salt");
}

void ALobbyHandler::NextOutfit(int OutfitIndex) {
    ++Selected.OutfitIndex[OutfitIndex];
    if(Selected.OutfitIndex[OutfitIndex] >= OutfitItemCount[Selected.BodyIndex][OutfitIndex]) {
        Selected.OutfitIndex[OutfitIndex] = 0;
    }
    Actor->SetOutfitMesh(OutfitIndex, GetSelectedOutfitMesh(OutfitIndex));
}

void ALobbyHandler::PrevOutfit(int OutfitIndex) {
    --Selected.OutfitIndex[OutfitIndex];
    if(Selected.OutfitIndex[OutfitIndex] < 0) {
        Selected.OutfitIndex[OutfitIndex] = OutfitItemCount[Selected.BodyIndex][OutfitIndex] - 1;
    }
    Actor->SetOutfitMesh(OutfitIndex, GetSelectedOutfitMesh(OutfitIndex));
}

void ALobbyHandler::BodySelect(int In, bool bPreInit) {
    Selected.BodyIndex = In;

    // 바디 메시 로드
    Actor->SetBodyMesh(GetSelectedBodyMesh());

    // 의상 바디에 맞춰 초기화
    for(int i = 0; i < EPO_OutfitCount; ++i) {
        Selected.OutfitIndex[i] *= static_cast<int>(bPreInit); // false: 의상 초기화
        Actor->SetOutfitMesh(i, GetSelectedOutfitMesh(i));
    }

    // Idle 모션 재생
    Actor->PlayAnimation(UPlayerMeshManager::Instance(this)->Assets[Selected.BodyIndex].Anim[EPA_Idle]);
}

USkeletalMesh* ALobbyHandler::GetSelectedBodyMesh() const {
    return UPlayerMeshManager::Instance(this)->Assets[Selected.BodyIndex].Body;
}

USkeletalMesh* ALobbyHandler::GetSelectedOutfitMesh(int OutfitIndex) const {
    const auto& Arr = UPlayerMeshManager::Instance(this)->Assets[Selected.BodyIndex].Outfit[OutfitIndex];
    int         Idx = Selected.OutfitIndex[OutfitIndex];
    return Arr[Idx];
}

// 0 ~ Max - 1
void ALobbyHandler::SetPreviewCharacter(int32 Index) {
    SelectIndex = Index; // Save

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
    Selected.OutfitIndex[EPO_Face] = Item->FaceIndex;
    Selected.OutfitIndex[EPO_Hair] = Item->HairIndex;
    Selected.OutfitIndex[EPO_Upper] = Item->UpperIndex;
    Selected.OutfitIndex[EPO_Lower] = Item->LowerIndex;
    Selected.OutfitIndex[EPO_Shoes] = Item->ShoesIndex;

    // pre-init
    BodySelect(Item->BodyIndex, true);
}

void ALobbyHandler::AddListView(const FString& Name, const FPlayerOutfit& MeshInfo) {
    check(UManager::IsUser(this));

    UPlayerListViewEntryData* Item = NewObject<UPlayerListViewEntryData>(this);
    Item->Index = ++SelectMax;
    Item->Name = Name;
    Item->BodyIndex = MeshInfo.BodyIndex;
    Item->HairIndex = MeshInfo.OutfitIndex[EPO_Hair];
    Item->FaceIndex = MeshInfo.OutfitIndex[EPO_Face];
    Item->UpperIndex = MeshInfo.OutfitIndex[EPO_Upper];
    Item->LowerIndex = MeshInfo.OutfitIndex[EPO_Lower];
    Item->ShoesIndex = MeshInfo.OutfitIndex[EPO_Shoes];
    LobbyUI->PlayerCharacterList->AddItem(Item);
}

void ALobbyHandler::LoadPlayerCharacterList(const FString& ID) {
    check(UManager::IsServer(this));
    if (ID.IsEmpty()) {
        check(false);
        return;
    }

    UDatabaseManager::Instance(this)->Query(
        "SELECT * FROM player_tbl WHERE owner_id = ? ORDER BY created_at ASC",
        [ID](sql::PreparedStatement* In) { In->setString(1, TCHAR_TO_UTF8(*ID)); },
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
            LoadCharactersResponse(Params);
        } // end lambda
    ); // end Query
}

void ALobbyHandler::OnLogIn() {
    FString ID = LoginUI->InputID->GetText().ToString();
    FString PW = LoginUI->InputPW->GetText().ToString();
    LogInToServer(ID, PW);
}

void ALobbyHandler::OnSignUp() {
    FString ID = LoginUI->InputID->GetText().ToString();
    FString PW = LoginUI->InputPW->GetText().ToString();
    SignUpToServer(ID, PW);
}

void ALobbyHandler::OnLogOut() {
    UE_LOG(LogTemp, Log, _T("Clicked"));
}

void ALobbyHandler::OnSignOut() {
    UE_LOG(LogTemp, Log, _T("Clicked"));
}
