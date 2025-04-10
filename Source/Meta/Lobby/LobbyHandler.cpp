#include "Lobby/LobbyHandler.h"
#include "Lobby/LobbyActor.h"

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

    MessageBoxUI = CreateWidget<UMessageBoxUI>(GetWorld(), MessageBoxUiClass);
    MessageBoxUI->AddToViewport(1);
    MessageBoxUI->Bind(this);
    MessageBoxUI->SetVisibility(ESlateVisibility::Hidden); // default: hidden

    // 에셋 개수 가져오기
    auto Manager = UPlayerMeshManager::Instance(this);
    for(int i = 0; i < EPB_Count; ++i) {
        for(int j = 0; j < EPL_Count; ++j) {
            OutfitItemCount[i][j] = Manager->Assets[i].LookMesh[j].Num();
        }
    }

    check(IsLocalPlayerController());

    // create actor
    Actor = GetWorld()->SpawnActor<ALobbyActor>(FVector{ 0 }, FRotator{ 0 });
    Viewer = GetWorld()->SpawnActor<ACameraActor>(FVector{ 0, 200, 90 }, FRotator{ 0, -90, 0 });
    check(Actor && Viewer);

    // set camera
    Viewer->GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Orthographic);
    Viewer->GetCameraComponent()->SetOrthoWidth(500.0f);
    SetViewTarget(Viewer);

    EnterLoginModeResponse(); // begin
}

void ALobbyHandler::BeginServer() {
    // not work
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

void ALobbyHandler::GetResultMessageResponse(int8 Code) {
    UManager::Dispatch([this, Code]() {
        RESPONSE(GetResultMessageToClient, Code);
    });
}

void ALobbyHandler::NewCharacterResponse(const FPlayerPreset& Preset) {
    UManager::Dispatch([this, Preset] {
        RESPONSE(NewCharacterToClient, Preset);
    });
}

void ALobbyHandler::LoadCharactersResponse(const TArray<FPlayerPreset>& Params) {
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
                // insert
                UManager::Dispatch([this, ID]() {
                    UClientSessionManager::Instance(this)->Enter(this,
                        UClientSessionManager::Create(ID)
                    );
                });

                LoadCharacterList(ID);    // get list
                EnterLobbyModeResponse(); // move
            }
            else GetResultMessageResponse(ERC_InvalidID); // print message
        } // end lambda
    ); // end Query
}

void ALobbyHandler::LogOutToServer_Implementation() {
    check(UManager::IsServer(this));
    UClientSessionManager::Instance(this)->Leave(this);
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
                        GetResultMessageResponse(ERC_CreatedID); // ok
                    }
                ); // end Query
            } else GetResultMessageResponse(ERC_DuplicatedD); // duplicated
        } // end lambda
    ); // end Query
}

void ALobbyHandler::SignOutToServer_Implementation(const FString& ID, const FString& PW) {
    // TODO:
}

void ALobbyHandler::NewCharacterToServer_Implementation(const FPlayerPreset& Preset) {
    check(UManager::IsServer(this));

    // DB에 등록하기 위해 플레이어 정보를 가져옵니다.
    FString ID = UClientSessionManager::Instance(this)->GetClientSession(this)->ID;

    UDatabaseManager::Instance(this)->Query(
        "SELECT * FROM player_tbl WHERE nickname = ? LIMIT 1", // search 1
        [Preset](sql::PreparedStatement* In) { In->setString(1, TCHAR_TO_UTF8(*Preset.Name)); },
        [this, ID, Preset](sql::ResultSet* In) {
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
                    [ID, Preset](sql::PreparedStatement* In) {
                        In->setString(1, TCHAR_TO_UTF8(*Preset.Name));
                        In->setString(2, TCHAR_TO_UTF8(*ID));
                        In->setInt(3, Preset.BodyCode);
                        In->setInt(4, Preset.LookCode[EPL_Face]);
                        In->setInt(5, Preset.LookCode[EPL_Hair]);
                        In->setInt(6, Preset.LookCode[EPL_Upper]);
                        In->setInt(7, Preset.LookCode[EPL_Lower]);
                        In->setInt(8, Preset.LookCode[EPL_Shoes]);
                    },
                    [this, Preset](sql::ResultSet*) {
                        GetResultMessageResponse(ERC_CreatedCharacter); // ok
                        NewCharacterResponse(Preset);
                    }
                );
            }
            else {
                GetResultMessageResponse(ERC_DuplicatedName); // duplicated
            }
        } // end process
    ); // end query
}

void ALobbyHandler::NewCharacterToClient_Implementation(const FPlayerPreset& Preset) {
    check(UManager::IsUser(this));

    AddCharacterToList(Preset);

    // 만든 캐릭터 정보로 로딩합니다.
    SelectIndex = SelectMax - 1;

    // 캐릭터 선택 창으로 UI 전환합니다.
    EnterLobbyModeResponse();
}

void ALobbyHandler::LoadCharactersToClient_Implementation(const TArray<FPlayerPreset>& In) {
    // not exist
    if(In.Num() == 0) {
        SelectCharacterFromList(-1); // set nullptr
        SelectMax = 0;
    } else {
        SelectMax = 0;
        for(auto& Param : In) {
            AddCharacterToList(Param);
        } // end for
    } // end else
}


void ALobbyHandler::EnterLoginModeToClient_Implementation() {
    check(UManager::IsUser(this));
    LoginUI->SetVisibility(ESlateVisibility::Visible);
    LobbyUI->SetVisibility(ESlateVisibility::Hidden);
    CharacterCustomizeUI->SetVisibility(ESlateVisibility::Hidden);
    Initialize();
}

void ALobbyHandler::EnterLobbyModeToClient_Implementation() {
    check(UManager::IsUser(this));
    LoginUI->SetVisibility(ESlateVisibility::Hidden);
    LobbyUI->SetVisibility(ESlateVisibility::Visible);
    CharacterCustomizeUI->SetVisibility(ESlateVisibility::Hidden);
    SelectCharacterFromList(0); // get first
}

void ALobbyHandler::EnterCustomizeModeToClient_Implementation() {
    check(UManager::IsUser(this));
    LoginUI->SetVisibility(ESlateVisibility::Hidden);
    LobbyUI->SetVisibility(ESlateVisibility::Hidden);
    CharacterCustomizeUI->SetVisibility(ESlateVisibility::Visible);

    CharacterCustomizeUI->NameInputBox->SetText(FText::GetEmpty());
    BodySelect(0); // set initial body and looks
}

void ALobbyHandler::GetResultMessageToClient_Implementation(int8 Code) {
    ShowMessageBox();
    MessageBoxUI->MessageBox->SetText(FText::FromString(GetResultMessage(Code)));
}

void ALobbyHandler::OnBodyNext() {
    if(++Selected.BodyCode >= EPB_Count) {
        Selected.BodyCode = 0;
    }
    BodySelect(Selected.BodyCode);
}

void ALobbyHandler::OnBodyPrev() {
    if(--Selected.BodyCode < 0) {
        Selected.BodyCode = EPB_Count - 1;
    }
    BodySelect(Selected.BodyCode);
}

void ALobbyHandler::OnCustomBegin() {
    check(UManager::IsUser(this));
    EnterCustomizeModeResponse();
}

void ALobbyHandler::OnCustomEnd() {
    Selected.Name = CharacterCustomizeUI->NameInputBox->GetText().ToString();
    NewCharacterToServer(Selected);
}

void ALobbyHandler::OnCustomCancel() {
    EnterLobbyModeResponse();
    SelectCharacterFromList(0); // 취소: 캐릭터가 존재하면 첫 캐릭터, 아니면 안 보임
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

void ALobbyHandler::Initialize() {
    LoginUI->InputID->SetText(FText::GetEmpty());
    LoginUI->InputPW->SetText(FText::GetEmpty());
    LobbyUI->PlayerCharacterList->ClearListItems();

    // hide
    Actor->SetBodyMesh(nullptr);
    for (int i = 0; i < EPL_Count; ++i) {
        Actor->SetLookMesh(i, nullptr);
    }
}

void ALobbyHandler::NextOutfit(int LookCode) {
    ++Selected.LookCode[LookCode];
    if(Selected.LookCode[LookCode] >= OutfitItemCount[Selected.BodyCode][LookCode]) {
        Selected.LookCode[LookCode] = 0;
    }
    Actor->SetLookMesh(LookCode, GetSelectedOutfitMesh(LookCode));
}

void ALobbyHandler::PrevOutfit(int LookCode) {
    --Selected.LookCode[LookCode];
    if(Selected.LookCode[LookCode] < 0) {
        Selected.LookCode[LookCode] = OutfitItemCount[Selected.BodyCode][LookCode] - 1;
    }
    Actor->SetLookMesh(LookCode, GetSelectedOutfitMesh(LookCode));
}

void ALobbyHandler::BodySelect(int In, bool bPreInit) {
    Selected.BodyCode = In;

    // 바디 메시 로드
    Actor->SetBodyMesh(GetSelectedBodyMesh());

    // 의상 바디에 맞춰 초기화
    for(int i = 0; i < EPL_Count; ++i) {
        Selected.LookCode[i] *= static_cast<int>(bPreInit); // false: 의상 초기화
        Actor->SetLookMesh(i, GetSelectedOutfitMesh(i));
    }

    // Idle 모션 재생
    Actor->PlayAnimation(UPlayerMeshManager::Instance(this)->Assets[Selected.BodyCode].Anim[EPA_Idle]);
}

USkeletalMesh* ALobbyHandler::GetSelectedBodyMesh() const {
    return UPlayerMeshManager::Instance(this)->Assets[Selected.BodyCode].Body;
}

USkeletalMesh* ALobbyHandler::GetSelectedOutfitMesh(int LookCode) const {
    const auto& Arr = UPlayerMeshManager::Instance(this)->Assets[Selected.BodyCode].LookMesh[LookCode];
    int         Idx = Selected.LookCode[LookCode];
    return Arr[Idx];
}

// 0 ~ Max - 1
void ALobbyHandler::SelectCharacterFromList(int32 Index) {
    SelectIndex = Index; // Save

    // 범위 밖이면 nullptr로 세팅합니다.
    if (Index >= SelectMax || Index < 0) {
        Actor->SetBodyMesh(nullptr);
        for (int i = 0; i < EPL_Count; ++i) {
            Actor->SetLookMesh(i, nullptr);
        }
        Actor->PlayAnimation(nullptr);
        return;
    }

    const TArray<UObject*>&   Items = LobbyUI->PlayerCharacterList->GetListItems();
    UPlayerListViewEntryData* Item  = Cast<UPlayerListViewEntryData>(Items[Index]);

    // load
    Selected.LookCode[EPL_Face]  = Item->FaceCode;
    Selected.LookCode[EPL_Hair]  = Item->HairCode;
    Selected.LookCode[EPL_Upper] = Item->UpperCode;
    Selected.LookCode[EPL_Lower] = Item->LowerCode;
    Selected.LookCode[EPL_Shoes] = Item->ShoesCode;

    // pre-init
    BodySelect(Item->BodyCode, true);
}

void ALobbyHandler::AddCharacterToList(const FPlayerPreset& Preset) {
    check(UManager::IsUser(this));

    UPlayerListViewEntryData* Item = NewObject<UPlayerListViewEntryData>(this);
    Item->Index     = ++SelectMax;
    Item->Name      = Preset.Name;
    Item->BodyCode =  Preset.BodyCode;
    Item->HairCode =  Preset.LookCode[EPL_Hair];
    Item->FaceCode =  Preset.LookCode[EPL_Face];
    Item->UpperCode = Preset.LookCode[EPL_Upper];
    Item->LowerCode = Preset.LookCode[EPL_Lower];
    Item->ShoesCode = Preset.LookCode[EPL_Shoes];
    LobbyUI->PlayerCharacterList->AddItem(Item);
}

void ALobbyHandler::LoadCharacterList(const FString& ID) {
    check(UManager::IsServer(this));
    if (ID.IsEmpty()) {
        check(false);
        return;
    }

    UDatabaseManager::Instance(this)->Query(
        "SELECT * FROM player_tbl WHERE owner_id = ? ORDER BY created_at ASC",
        [ID](sql::PreparedStatement* In) { In->setString(1, TCHAR_TO_UTF8(*ID)); },
        [this](sql::ResultSet* In) {
            TArray<FPlayerPreset> Params;
            while (In->next()) {
                FPlayerPreset Temp;
                Temp.Name                = UTF8_TO_TCHAR(In->getString("nickname").c_str());
                Temp.BodyCode            = In->getInt("body_type");
                Temp.LookCode[EPL_Face]  = In->getInt("face_type");
                Temp.LookCode[EPL_Hair]  = In->getInt("hair_type");
                Temp.LookCode[EPL_Upper] = In->getInt("upper_type");
                Temp.LookCode[EPL_Lower] = In->getInt("lower_type");
                Temp.LookCode[EPL_Shoes] = In->getInt("shoes_type");
                Params.Add(Temp);
            }
            LoadCharactersResponse(Params);
        } // end lambda
    ); // end Query
}

void ALobbyHandler::ShowMessageBox() {
    check(!UManager::IsDedicated(this));
    MessageBoxUI->SetVisibility(ESlateVisibility::Visible);
}

void ALobbyHandler::HideMessageBox() {
    check(!UManager::IsDedicated(this));
    MessageBoxUI->SetVisibility(ESlateVisibility::Hidden);
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
    LogOutToServer();
    EnterLoginModeResponse(); // return
}

void ALobbyHandler::OnSignOut() {
    UE_LOG(LogTemp, Log, _T("Clicked"));
}
