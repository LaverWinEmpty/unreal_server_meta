#include "Lobby/LobbyHandler.h"
#include "Lobby/LobbyActor.h"

#include "UI/MessageBoxUI.h"
#include "UI/InputBoxUI.h"
#include "UI/LoginUI.h"
#include "UI/LobbyUI.h"
#include "UI/CharacterCustomizeUI.h"
#include "UI/CharacterListEntryData.h"
#include "UI/RoomListEntryData.h"

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

#include "Kismet/GameplayStatics.h"


#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include "openssl/evp.h"
THIRD_PARTY_INCLUDES_END
#undef UI
#include "openssl/sha.h"

ALobbyHandler::ALobbyHandler() {
    bShowMouseCursor = true;
    bReplicates      = true;

#if !UE_SERVER
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

    // InputBox UI
    static ConstructorHelpers::FClassFinder<UUserWidget> InputBoxUiFinder(TEXT("/Game/UI/BP_InputBoxUI"));
    check(InputBoxUiFinder.Class);
    InputBoxUiClass = InputBoxUiFinder.Class;
#endif

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

    // MessageBox UI 생성, 자동 바인드
    MessageBoxUI = CreateWidget<UMessageBoxUI>(GetWorld(), MessageBoxUiClass);
    MessageBoxUI->AddToViewport(1);
    MessageBoxUI->SetVisibility(ESlateVisibility::Hidden); // default: hidden

    // CreateRoom UI 생성 후 바인드
    CreateRoomUI = CreateWidget<UInputBoxUI>(GetWorld(), InputBoxUiClass);
    CreateRoomUI->AddToViewport(2);
    CreateRoomUI->OkButton->OnClicked.AddDynamic(this, &ThisClass::OnCreateRoom);
    CreateRoomUI->SetVisibility(ESlateVisibility::Hidden); // default: hidden

    // 에셋 개수 가져오기
    auto Manager = UPlayerMeshManager::Instance(this);
    for(int i = 0; i < EPB_Count; ++i) {
        for(int j = 0; j < EPL_Count; ++j) {
            LookKinds[i][j] = Manager->Assets[i].LookMesh[j].Num();
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
        EnterLobbyModeToClient();
    });
}

void ALobbyHandler::EnterLoginModeResponse() {
    UManager::Dispatch([this]() {
        EnterLoginModeToClient();
    });
}

void ALobbyHandler::EnterCustomizeModeResponse() {
    UManager::Dispatch([this]() {
        EnterCustomizeModeToClient();
    });
}

void ALobbyHandler::PrintResultMessageResponse(int8 Code) {
    UManager::Dispatch([this, Code]() {
        PrintResultMessageToClient(Code);
    });
}

void ALobbyHandler::CreateRoomToServer_Implementation(const FString& Name, const FString& IP) {
    check(UManager::IsServer(this));

    // TODO: RoomManager로 리펙터링
    RoomIpList.Add(IP);
    RoomNameList.Add(Name);

    // response 합니다
    UpdateRoomListResponse(Name, IP);
}

void ALobbyHandler::CreateRoomRequest(const FString& Name, const FString& IP) {
    UManager::Dispatch([this, Name, IP]() {
        CreateRoomToServer(Name, IP);
    });
}

void ALobbyHandler::UpdateRoomListToClient_Implementation(const FString& Name, const FString& IP) {
    if (!LobbyUI) {
        return;
    }

    URoomListEntryData* Data = NewObject<URoomListEntryData>(this);
    Data->Name    = Name;
    Data->Address = IP;
    LobbyUI->RoomList->AddItem(Data);
}

void ALobbyHandler::UpdateRoomListResponse(const FString& Name, const FString& IP) {
    UManager::Dispatch([this, Name, IP]() {
        UpdateRoomListToClient(Name, IP);
    });
}

void ALobbyHandler::InitializeRoomListToClient_Implementation() {
    //TODO:
}

void ALobbyHandler::InitializeRoomListResponse() {
    //TODO:
}

void ALobbyHandler::NewCharacterResponse(const FPlayerPreset& Preset) {
    UManager::Dispatch([this, Preset] {
        NewCharacterToClient(Preset);
    });
}

void ALobbyHandler::LoadCharactersResponse(const TArray<FPlayerPreset>& Params) {
    UManager::Dispatch([this, Params] {
        LoadCharactersToClient(Params);
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

                PostLogIn(ID); // get list -> response when end query
            }
            else PrintResultMessageResponse(ERC_UnknownID); // print message
        } // end lambda
    ); // end Query
}

void ALobbyHandler::LogInRequest(const FString& ID, const FString& PW) {
    UManager::Dispatch([this, ID, PW] {
        LogInToServer(ID, PW);
    });
}

void ALobbyHandler::LogOutToServer_Implementation() {
    check(UManager::IsServer(this));
    UClientSessionManager::Instance(this)->Leave(this);
}

void ALobbyHandler::LogOutRequest() {
    UManager::Dispatch([this] {
        LogOutToServer();
    });
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
                        PrintResultMessageResponse(ERC_CreatedAccount); // ok
                    }
                ); // end Query
            } else PrintResultMessageResponse(ERC_DuplicatedD); // duplicated
        } // end lambda
    ); // end Query
}

bool ALobbyHandler::SignUpToServer_Validate(const FString& ID, const FString& PW) {
    if (ID.Len() < 3 || ID.Len() > 16) {
        PrintResultMessageResponse(ERC_InvalidID); // invalid id
        return false;
    }
    if (PW.Len() < 3) {
        PrintResultMessageResponse(ERC_InvalidPW); // invalid pw
        return false;
    }
    return true;
}

void ALobbyHandler::SignUpRequest(const FString& ID, const FString& PW) {
    UManager::Dispatch([this, ID, PW](){
        SignUpToServer(ID, PW);
    });
}

void ALobbyHandler::SignOutToServer_Implementation(const FString& ID, const FString& PW) {
    // TODO:
}

void ALobbyHandler::SignOutRequest(const FString& ID, const FString& PW) {
    UManager::Dispatch([this, ID, PW] {
        SignOutToServer(ID, PW);
    });
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
                        PrintResultMessageResponse(ERC_CreatedCharacter); // ok
                        NewCharacterResponse(Preset);
                    }
                );
            }
            else {
                PrintResultMessageResponse(ERC_DuplicatedName); // duplicated
            }
        } // end process
    ); // end query
}

bool ALobbyHandler::NewCharacterToServer_Validate(const FPlayerPreset& Param) {
    if (Param.Name.IsEmpty() || Param.Name.Len() > 8) {
        PrintResultMessageResponse(ERC_InvalidName); // invalid name
        return false;
    }
    return true;
}

void ALobbyHandler::NewCharacterRequest(const FPlayerPreset& Param) {
    UManager::Dispatch([this, Param] {
        NewCharacterToServer(Param);
    });
}

void ALobbyHandler::NewCharacterToClient_Implementation(const FPlayerPreset& Preset) {
    check(UManager::IsUser(this));

    PostNewCharacter(Preset);

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
            PostNewCharacter(Param);
        } // end for
    } // end else
}


void ALobbyHandler::EnterLoginModeToClient_Implementation() {
    UE_LOG(LogTemp, Log, _T("Try EnterLoginMode"));
    checkf(UManager::IsUser(this), _T("NetMode [%s]"), UManager::GetNetModeString(this));

    UE_LOG(LogTemp, Log, _T("Set UI SetVisibility"));
    LoginUI->SetVisibility(ESlateVisibility::Visible);
    LobbyUI->SetVisibility(ESlateVisibility::Hidden);
    CharacterCustomizeUI->SetVisibility(ESlateVisibility::Hidden);

    UE_LOG(LogTemp, Log, _T("Call Initialize"));
    Initialize();
    UE_LOG(LogTemp, Log, _T("Succeeded EnterLoginMode"));
}

void ALobbyHandler::EnterLobbyModeToClient_Implementation() {
    UE_LOG(LogTemp, Log, _T("Try EnterLobbyMode"));
    checkf(UManager::IsUser(this), _T("NetMode [%s]"), UManager::GetNetModeString(this));

    UE_LOG(LogTemp, Log, _T("Set UI SetVisibility"));
    LoginUI->SetVisibility(ESlateVisibility::Hidden);
    LobbyUI->SetVisibility(ESlateVisibility::Visible);
    CharacterCustomizeUI->SetVisibility(ESlateVisibility::Hidden);

    UE_LOG(LogTemp, Log, _T("Call SelectCharacterFromList"));
    SelectCharacterFromList(0); // get first
    UE_LOG(LogTemp, Log, _T("Succeeded EnterLobbyMode"));
}

void ALobbyHandler::EnterCustomizeModeToClient_Implementation() {
    UE_LOG(LogTemp, Log, _T("Try EnterCustomizeMode"));
    checkf(UManager::IsUser(this), _T("NetMode [%s]"), UManager::GetNetModeString(this));

    UE_LOG(LogTemp, Log, _T("Set UI SetVisibility"));
    LoginUI->SetVisibility(ESlateVisibility::Hidden);
    LobbyUI->SetVisibility(ESlateVisibility::Hidden);
    CharacterCustomizeUI->SetVisibility(ESlateVisibility::Visible);
    CharacterCustomizeUI->NameInputBox->SetText(FText::GetEmpty());

    UE_LOG(LogTemp, Log, _T("Call BodySelect"));
    BodySelect(0); // set initial body and looks
    UE_LOG(LogTemp, Log, _T("Succeeded EnterCustomizeMode"));
}

void ALobbyHandler::PrintResultMessageToClient_Implementation(int8 Code) {
    MessageBoxUI->Show();
    MessageBoxUI->MessageBlock->SetText(FText::FromString(GetResultMessageFromCode(Code)));
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
    NewCharacterRequest(Selected);
}

void ALobbyHandler::OnCustomCancel() {
    EnterLobbyModeResponse();
    SelectCharacterFromList(0); // 취소: 캐릭터가 존재하면 첫 캐릭터, 아니면 안 보임
}

void ALobbyHandler::OnStart() {
    // TODO:
    UE_LOG(LogTemp, Log, _T("Clicked"));
}

void ALobbyHandler::OnShowUICreateRoom() {
    check(UManager::IsUser(this));
    CreateRoomUI->Show();
}

void ALobbyHandler::OnCreateRoom() {
    // TODO: IP 데디 RoomManager에 저장...

// Error: Standalone
    if (UManager::IsStandalone(this)) {
        UGameplayStatics::OpenLevel(GetWorld(), "TestLevel", false, "listen");
        return;
    }

    FString Address = GetNetConnection()->LowLevelGetRemoteAddress();

    // port set default
    FString IP;
    if (Address.Split(TEXT(":"), &IP, nullptr)) {
        Address = FString::Printf(_T("%s:%d"), *IP, 7777);
    }

    // TODO: 소켓을 연결합니다.
    // TODO: 소켓은 RoomManager에서 관리합니다.
    // TODO: RoomManager는 Thread를 하나 만들어야 합니다

    UE_LOG(LogTemp, Log, _T("Room Create Request: %s"), *Address);

    // 로그아웃 요청
    LogOutRequest();

    // 방생성 요청
    FString Name = CreateRoomUI->InputBlock->GetText().ToString();
    CreateRoomRequest(Name, IP);

    //// 리슨 서버를 클라이언트에서 생성합니다
    //FString Params = "TestLevel -server -port=8888"; // 임시
    //FPlatformProcess::CreateProc(
    //    *FString("MetaListen.exe"),
    //    *Params,
    //    true, false, false, nullptr, 0, nullptr, nullptr
    //);

    // OpenLevel(..., "listen");
    ClientTravel("ListenLevel", ETravelType::TRAVEL_Absolute);
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
    check(UManager::IsUser(this));

    LoginUI->InputID->SetText(FText::GetEmpty());
    LoginUI->InputPW->SetText(FText::GetEmpty());
    LobbyUI->PlayerCharacterList->ClearListItems();

    // hide
    Actor->SetBodyMesh(nullptr);
    for (int i = 0; i < EPL_Count; ++i) {
        Actor->SetLookMesh(i, nullptr);
    }
}

void ALobbyHandler::NextLook(int LookCode) {
    check(UManager::IsUser(this));

    ++Selected.LookCode[LookCode];
    if(Selected.LookCode[LookCode] >= LookKinds[Selected.BodyCode][LookCode]) {
        Selected.LookCode[LookCode] = 0;
    }
    Actor->SetLookMesh(LookCode, GetSelectedLookMesh(LookCode));
}

void ALobbyHandler::PrevLook(int LookCode) {
    check(UManager::IsUser(this));

    --Selected.LookCode[LookCode];
    if(Selected.LookCode[LookCode] < 0) {
        Selected.LookCode[LookCode] = LookKinds[Selected.BodyCode][LookCode] - 1;
    }
    Actor->SetLookMesh(LookCode, GetSelectedLookMesh(LookCode));
}

void ALobbyHandler::BodySelect(int In, bool bPreInit) {
    UE_LOG(LogTemp, Log, _T("Try BodySelect"));
    check(UManager::IsUser(this));
    UE_LOG(LogTemp, Log, _T("Failed")); // 여기 확인하려고 그냥 LOG Failed

    Selected.BodyCode = In;
    if (!Actor) {
        UE_LOG(LogTemp, Log, _T("Actor is NullPointerException"));
    }

    // 바디 메시 로드
    Actor->SetBodyMesh(GetSelectedBodyMesh()); // 이거 시발 오류같다

    // 의상 바디에 맞춰 초기화
    for(int i = 0; i < EPL_Count; ++i) {
        Selected.LookCode[i] *= static_cast<int>(bPreInit); // false: 의상 초기화

        // bPreInit 즉 기존에 세팅하고 유지한 값이 바뀐 바디의 룩 코드 값을 넘기면 체크
        if (Selected.LookCode[i] >= LookKinds[Selected.BodyCode][i]) {
            UE_LOG(LogTemp, Log, _T("LookCode Out Of Range, %d > %d"), Selected.LookCode[i], LookKinds[Selected.BodyCode][i]);
        }

        Actor->SetLookMesh(i, GetSelectedLookMesh(i));
    }

    // Idle 모션 재생
    Actor->PlayAnimation(UPlayerMeshManager::Instance(this)->Assets[Selected.BodyCode].Anim[EPA_Idle]);
}

USkeletalMesh* ALobbyHandler::GetSelectedBodyMesh() const {
    check(UManager::IsUser(this));

    // TODO: 여기 검사

    return UPlayerMeshManager::Instance(this)->Assets[Selected.BodyCode].Body;
}

USkeletalMesh* ALobbyHandler::GetSelectedLookMesh(int LookCode) const {
    check(UManager::IsUser(this));

    const auto& Arr = UPlayerMeshManager::Instance(this)->Assets[Selected.BodyCode].LookMesh[LookCode];
    int         Idx = Selected.LookCode[LookCode];

    checkf(Arr[Idx], _T("Mesh[%d][%d] Is NullPointerException"), LookCode, Idx);
    return Arr[Idx];
}

// 0 ~ Max - 1
void ALobbyHandler::SelectCharacterFromList(int32 Index) {
    UE_LOG(LogTemp, Log, _T("Try SelectCharacterFromList"));

    SelectIndex = Index; // Save

    // 범위 밖이면 nullptr로 세팅합니다.
    if (Index >= SelectMax || Index < 0) {
        UE_LOG(LogTemp, Log, _T("Character List Empty"));

        LobbyUI->PlayerNameText->SetText(FText::GetEmpty());

        Actor->SetBodyMesh(nullptr);
        for (int i = 0; i < EPL_Count; ++i) {
            Actor->SetLookMesh(i, nullptr);
        }
        Actor->PlayAnimation(nullptr);
        return;
    }

    const TArray<UObject*>& Items = LobbyUI->PlayerCharacterList->GetListItems();
    if (Items.Num() == 0) {
        UE_LOG(LogTemp, Log, _T("Player Character List Item Load Failed."));
        check(false);
    }

    UCharacterListEntryData* Item  = Cast<UCharacterListEntryData>(Items[Index]);
    if (!Item) {
        UE_LOG(LogTemp, Log, _T("Player Character List Item Casting Failed."))
        check(false);
    }

    // set name
    if (!LobbyUI) {
        UE_LOG(LogTemp, Log, _T("LOBBY UI IS WHY NULLPTR????"))
        check(false);
    }
    LobbyUI->PlayerNameText->SetText(FText::FromString(Item->Name));

    // load
    Selected.LookCode[EPL_Face]  = Item->FaceCode;
    Selected.LookCode[EPL_Hair]  = Item->HairCode;
    Selected.LookCode[EPL_Upper] = Item->UpperCode;
    Selected.LookCode[EPL_Lower] = Item->LowerCode;
    Selected.LookCode[EPL_Shoes] = Item->ShoesCode;

    // pre-init
    
    UE_LOG(LogTemp, Log, _T("Call BodySelect"));
    BodySelect(Item->BodyCode, true);
}

void ALobbyHandler::PostNewCharacter(const FPlayerPreset& Preset) {
    check(UManager::IsUser(this));

    UCharacterListEntryData* Item = NewObject<UCharacterListEntryData>(this);
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

void ALobbyHandler::PostLogIn(const FString& ID) {
    check(UManager::IsServer(this));
    if (ID.IsEmpty()) {
        check(false);
        return;
    }

    UE_LOG(LogTemp, Log, _T("LogIn Succeeded, PostLogIn()"));

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

            UE_LOG(LogTemp, Log, _T("SQL: Load Characters Succeeded"));

            LoadCharactersResponse(Params); // get list
            UE_LOG(LogTemp, Log, _T("LoadCharactersResponse() Succeeded"));

            EnterLobbyModeResponse();       // move
            UE_LOG(LogTemp, Log, _T("EnterLobbyModeResponse() Succeeded"));
        } // end lambda
    ); // end Query
}

void ALobbyHandler::OnLogIn() {
    FString ID = LoginUI->InputID->GetText().ToString();
    FString PW = LoginUI->InputPW->GetText().ToString();
    LogInRequest(ID, PW);
}

void ALobbyHandler::OnSignUp() {
    FString ID = LoginUI->InputID->GetText().ToString();
    FString PW = LoginUI->InputPW->GetText().ToString();
    SignUpRequest(ID, PW);
}

void ALobbyHandler::OnLogOut() {
    LogOutRequest();
    EnterLoginModeResponse(); // return
}

void ALobbyHandler::OnSignOut() {
    UE_LOG(LogTemp, Log, _T("Clicked"));
}
