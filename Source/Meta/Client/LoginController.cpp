#include "Client/LoginController.h"
#include "Server/DatabaseManager.h"
#include "Server/ClientSessionManager.h"
#include "UI/LoginUI.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include "openssl/evp.h"
THIRD_PARTY_INCLUDES_END
#undef UI
#include "openssl/sha.h"

ALoginController::ALoginController() {
    // duplicated 오류 수정해야됨
    static ConstructorHelpers::FClassFinder<UUserWidget> Finder(TEXT("/Game/Assets/UI/BP_LoginUI"));
    check(Finder.Class);
    LoginWidgetClass = Finder.Class;
}

void ALoginController::BeginPlay() {
    // 마우스 보이게
    GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;

    // 로그인 위젯 생성
    Widget = CreateWidget<ULoginUI>(GetWorld(), LoginWidgetClass);
    if (!Widget) {
        check(false);
    }
    Widget->AddToViewport();

    // 로그인 버튼 바인드
    if (Widget->LogInButton) {
        Widget->LogInButton->OnClicked.AddDynamic(this, &ALoginController::OnLogIn);
        Widget->SignUpButton->OnClicked.AddDynamic(this, &ALoginController::OnSignUp);
    }
}

void ALoginController::OnLogIn() {
    FString ID = Widget->InputID->GetText().ToString();
    FString PW = Widget->InputPW->GetText().ToString();
    Authenticate(EAA_LogIn, ID, PW);
}

void ALoginController::OnSignUp() {
    FString ID = Widget->InputID->GetText().ToString();
    FString PW = Widget->InputPW->GetText().ToString();
    Authenticate(EAA_SignUp, ID, PW);
}

void ALoginController::OnAuthenticate(int8 Type, int8 Result) {
    check(Type != EAA_LogOut) // 현재 로그인 상태가 아님

    // has error
    if (Result) {
        Widget->ResultText->SetColorAndOpacity(FColor::Red);
    }

    // 로그인의 경우레벨 변경됨
    else if (Type != EAA_LogIn) {
        Widget->ResultText->SetColorAndOpacity(FColor::White);
    }

    // 결과 출력
    Widget->ResultText->SetText(
        FText::FromString(GetResultMessage(Result))
    );
}

void ALoginController::Authenticate(EAuthAction Type, const FString& ID, const FString& PW) {
    UE_LOG(LogTemp, Log, TEXT("서버에 계정관리 요청을 보냅니다"));

    // server to client
    if (UManager::IsClient(this)) {
        UE_LOG(LogTemp, Log, TEXT("call ALoginController::Authenticate() by Client"));
        AuthenticateRequest(static_cast<uint8>(Type), ID, PW); // client to server
        return;
    }

    // else is server
    // admin 등의 이유로 check 하지 않습니다.

    if (UManager::IsListen(this)) {
        UE_LOG(LogTemp, Log, TEXT("call ALoginController::Authenticate() by Listen"));
    }
    else if (UManager::IsStandalone(this)) {
        UE_LOG(LogTemp, Log, TEXT("call ALoginController::Authenticate() by Standalone"));
    }
    else if (UManager::IsDedicated(this)) {
        UE_LOG(LogTemp, Log, TEXT("call ALoginController::Authenticate() by Dedicate Server"));
    }
    else checkNoEntry();
    AuthenticateRequest_Implementation(Type, ID, PW); // dierct call
}

void ALoginController::AuthenticateRequest_Implementation(int8 Type, const FString& ID, const FString& PW) {
    check(UManager::IsServer(this)); // Request의 실제는 서버에서 처리

    switch (Type) {
    case EAA_LogIn:   LogIn(ID, PW);   break;
    case EAA_LogOut:  LogOut(ID, PW);  break;
    case EAA_SignUp:  SignUp(ID, PW);  break;
    case EAA_SignOut: SignOut(ID, PW); break;
    }
}

void ALoginController::AuthenticateResponse_Implementation(int8 Type, int8 Result) {
    check(UManager::IsUser(this)); // Request의 실제는 사용자쪽에서 처리

    UE_LOG(LogTemp, Log, TEXT("서버에서 클라이언트로 결과를 보냅니다"));
    if (UManager::IsDedicated(this)) {
        UE_LOG(LogTemp, Error, TEXT("응답 처리 함수가 DedicatedMode에서 호출되었습니다."));
        check(false);
    }

    ALoginController* PC = Cast<ALoginController>(UManager::GetWorldSafe(this)->GetFirstPlayerController());
    if (!PC) {
        UE_LOG(LogTemp, Error, TEXT("해당 클라이언트의 Controller가 LoginController가 아닙니다."));
        check(false);
        return;
    }
    // 테스트
    UE_LOG(LogTemp, Warning, TEXT("%s"), *PC->GetName());

    PC->OnAuthenticate(Type, Result);
}

void ALoginController::LogIn(const FString& ID, const FString& PW) {
    FString HashedPW = PasswordSHA256(PW);

    // Check account
    UDatabaseManager::Instance(this)->Query(
        "SELECT * FROM user_tbl WHERE user_id = ? and user_pw = ?",
        [=](sql::PreparedStatement* In) {
            In->setString(1, TCHAR_TO_UTF8(*ID));
            In->setString(2, TCHAR_TO_UTF8(*HashedPW));
        },
        [this, ID](sql::ResultSet* In) {
            EResultCode Result = ERC_AlreadyExistID;
            if (In->next()) {
                Result = ERC_Succeeded;
            }
            // GameThread에서 이어서 처리
            // 싱글턴이니까 안전할 거라 믿고 this call
            AsyncTask(ENamedThreads::GameThread,
                [this, ID, Result]() {
                    PostAuthenticate(ID, EAA_LogIn, Result);
                } // end labda
            ); // and AysncTasck
        } // end lambda
    ); // end Query
}

void ALoginController::LogOut(const FString& ID, const FString& PW) {
}

void ALoginController::SignUp(const FString& ID, const FString& PW) {
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
            if (In->next()) {
                AsyncTask(ENamedThreads::GameThread,
                    [this, ID]() {
                        PostAuthenticate(ID, EAA_SignUp, ERC_AlreadyExistID); // server to client
                    }
                );
            }
            // Insert account
            else {
                UDatabaseManager::Instance(this)->Query(
                    "INSERT INTO user_tbl VALUES (?, ?)",
                    [=](sql::PreparedStatement* In) {
                        In->setString(1, TCHAR_TO_UTF8(*ID));
                        In->setString(2, TCHAR_TO_UTF8(*HashedPW));
                    },
                    [this, ID](sql::ResultSet*) {
                        // GameThread에서 이어서 처리
                        // 싱글턴이니까 life cycle 안전할 거라 믿고 this call
                        AsyncTask(ENamedThreads::GameThread,
                            [this, ID]() {
                                PostAuthenticate(ID, EAA_SignUp, ERC_Succeeded); // server to client
                            } // end labda
                        ); // and AysncTasck
                    } // end lambda
                ); // end Query
            } // end else
        } // end lambda
    ); // end Query
}

void ALoginController::SignOut(const FString& ID, const FString& PW) {
    // TODO: DB UPDATE - 
    return;
}

void ALoginController::PostAuthenticate(const FString& ID, int8 Type, int8 Result) {
    // 응답처리
    UE_LOG(LogTemp, Log, TEXT("Call PostAuthenticate"));

    // TODO: 이 함수 진입 시점에서 스레드 안전한지 검사 필요함

    if (Result == ERC_Succeeded) {
        // 서버에서 send 전에 미리 처리할 작업을 여기서 진행합니다.
        switch (Type) {
        case EAA_LogIn:
            // 유저라면 OpenLevel
            if (UManager::IsUser(this)) {
                UManager::GetWorldSafe(this)-> // 클라이언트 World의
                    GetFirstPlayerController()-> // 컨트롤러의
                    ClientTravel("LobbyLevel", ETravelType::TRAVEL_Absolute);
            }
            else check(false); // dedicated 에서 호출됨
            UClientSessionManager::Instance(this)->OnLogIn(this, ID); // Log-in
            break;
        case EAA_LogOut:
            // UClientSessionManager::Instance(this)->OnLogOut(ID); // Log-out
            break;
        case EAA_SignUp: break;
        case EAA_SignOut: break;
        default:
            checkNoEntry();
        }
    }

    AuthenticateResponse(Type, Result); // Send
}

FString ALoginController::PasswordSHA256(const FString& In) {
    // salting and convert
    FString PW = GetSalted(In);
    std::string UTF8String = TCHAR_TO_UTF8(*PW);
    const char* InputData = UTF8String.c_str();
    size_t InputLength = UTF8String.length();

    // buffer
    unsigned char Buffer[SHA256_DIGEST_LENGTH];

    // hasing
    SHA256_CTX Sha256Context;
    SHA256_Init(&Sha256Context);
    SHA256_Update(&Sha256Context, InputData, InputLength);
    SHA256_Final(Buffer, &Sha256Context);

    // convert to hex
    FString HashString;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        HashString += FString::Printf(TEXT("%02X"), Buffer[i]);
    }
    return HashString;
}

FString ALoginController::GetSalted(const FString& In) {
    // 시늉만
    return In + _T("_Unreal_Meta_Project_Account_Manager_Salt");
}

