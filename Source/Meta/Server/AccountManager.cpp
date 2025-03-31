#include "AccountManager.h"
#include "Server/DatabaseManager.h"
#include "Actor/GenericGame.h"
#include "Actor/GenericCharacter.h"
#include "Actor/GenericController.h"
#include "Misc/SecureHash.h"
// #include "EncryptionContextOpenSSL.h"

#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include "openssl/evp.h"
THIRD_PARTY_INCLUDES_END
#undef UI
#include "openssl/sha.h"

// TODO: 유틸로 만들기
// TMap<FString, FStringFormatArg> Args;
// Args.Emplace(TEXT("0"), Code);
// return FString::Format(TEXT("Error {0}"), Args);

FString UAccountManager::ConvertActionTypeName(int8 In) {
    return ConvertActionTypeName(static_cast<EAuthAction>(In));
}

FString UAccountManager::GetResultMessage(int8 In) {
    return GetResultMessage(static_cast<EAuthResult>(In));
}

FString UAccountManager::ConvertActionTypeName(EAuthAction In) {
    switch(In) {
        case EAA_LogIn:   return _T("Log-in");
        case EAA_LogOut:  return _T("Log-out");
        case EAA_SignUp:  return _T("Sign-in");
        case EAA_SignOut: return _T("Sign-out");
    };
    checkNoEntry();
    return "";
}

FString UAccountManager::GetResultMessage(EAuthResult In) {
    switch (In) {
        case EAR_Uknown:       return _T("Unknown Error");
        case EAR_Suceeded:     return _T("작업 완료");
        case EAR_AlreadyExist: return _T("이미 존재하는 계정입니다");
        case EAR_NotExist:     return _T("존재하지 않는 계정입니다");
    }
    checkNoEntry();
    return "";
}

void UAccountManager::Authenticate(EAuthAction Type, const FString& ID, const FString& PW) {
    UE_LOG(LogTemp, Log, TEXT("서버에 계정관리 요청을 보냅니다"));

    // server to client
    if (IsClient()) {
        UE_LOG(LogTemp, Log, TEXT("call UAccountManager::Authenticate() by Client"));
        AuthenticateRequest(static_cast<uint8>(Type), ID, PW); // client to server
        return;
    }

    // else is server
    // admin 등의 이유로 check 하지 않습니다.

    if (IsListen()) {
        UE_LOG(LogTemp, Log, TEXT("call UAccountManager::Authenticate() by Listen"));
    }
    else if (IsStandalone()) {
        UE_LOG(LogTemp, Log, TEXT("call UAccountManager::Authenticate() by Standalone"));
    }
    else if (IsDedicated()) {
        UE_LOG(LogTemp, Log, TEXT("call UAccountManager::Authenticate() by Dedicate Server"));
    }
    else checkNoEntry();
    AuthenticateRequest_Implementation(Type, ID, PW); // dierct call
}

void UAccountManager::AuthenticateRequest_Implementation(int8 Type, const FString& ID, const FString& PW) {
    switch(Type) {
        case EAA_LogIn:   LogIn(ID, PW);   break;
        case EAA_LogOut:  LogOut(ID, PW);  break;
        case EAA_SignUp:  SignUp(ID, PW);  break;
        case EAA_SignOut: SignOut(ID, PW); break;
    }
}


void UAccountManager::AuthenticateResponse_Implementation(int8 Type, int8 Result) {
    UE_LOG(LogTemp, Log, TEXT("서버에서 클라이언트로 결과를 보냅니다"));
    if (IsClient()) {
        UE_LOG(LogTemp, Error, TEXT("응답 함수가 ClientMode에서 호출되었습니다."));
        check(false);
    }

    if (IsDedicated()) {
        UE_LOG(LogTemp, Error, TEXT("응답 처리 함수가 DedicatedMode에서 호출되었습니다."));
        check(false);
    }

    AGenericController* PC = Cast<AGenericController>(UManager::GetWorldSafe(this)->GetFirstPlayerController());
    if (!PC) {
        UE_LOG(LogTemp, Error, TEXT("해당 클라이언트의 Controller가 GenericController가 아닙니다."));
        check(false);
        return;
    }
    // 테스트
    UE_LOG(LogTemp, Warning, TEXT("%s"), *PC->GetName());

    PC->OnAuthenticate(Type, Result);
}

void UAccountManager::LogIn(const FString& ID, const FString& PW) {
    FString HashedPW = PasswordSHA256(PW);

    // Check account
    UManager::Instance<UDatabaseManager>(this)->Query(
        "SELECT * FROM user_tbl WHERE user_id = ? and user_pw = ?",
        [=](sql::PreparedStatement* In) {
            In->setString(1, TCHAR_TO_UTF8(*ID));
            In->setString(2, TCHAR_TO_UTF8(*HashedPW));
        },
        [&](sql::ResultSet* In) {
            EAuthResult Result = EAR_NotExist;
            if (In->next()) {
                Result = EAR_Suceeded;
            }
            PostAuthenticate(EAA_LogIn, Result); // 싱글턴이니까 안전할 거라 믿고 this call
        }
    );
}

void UAccountManager::LogOut(const FString& ID, const FString& PW) {
}

void UAccountManager::SignUp(const FString& ID, const FString& PW) {
    FString HashedPW = PasswordSHA256(PW);

    EAuthResult Result = EAR_Uknown;

    // Check account
    UManager::Instance<UDatabaseManager>(this)->Query(
        "SELECT * FROM user_tbl WHERE user_id = ? and user_pw = ?",
        [=](sql::PreparedStatement* In) {
            In->setString(1, TCHAR_TO_UTF8(*ID));
            In->setString(2, TCHAR_TO_UTF8(*HashedPW));
        },
        // callback: 매니저 클래스니까 믿고 this 캡쳐
        [&](sql::ResultSet* In) {
            if (In->next()) {
                PostAuthenticate(EAA_SignUp, EAR_AlreadyExist); // server to client
            }
            // Insert account
            else {
                UManager::Instance<UDatabaseManager>(this)->Query(
                    "INSERT INTO user_tbl VALUES (?, ?)",
                    [&](sql::PreparedStatement* In) {
                        In->setString(1, TCHAR_TO_UTF8(*ID));
                        In->setString(2, TCHAR_TO_UTF8(*HashedPW));
                    },
                    [&](sql::ResultSet*) {
                        PostAuthenticate(EAA_SignUp, EAR_Suceeded); // server to client
                    }
                );
            }
        }
    );
}

void UAccountManager::SignOut(const FString& ID, const FString& PW) {
    // TODO: DB UPDATE - 
    return;
}

void UAccountManager::PostAuthenticate(int8 Type, int8 Result) {
    // 응답처리
    UE_LOG(LogTemp, Error, TEXT("Call PostAuthenticate"));

    // TODO: 이 함수 진입 시점에서 스레드 안전한지 검사 필요함
    
    // 서버에서 send 전에 미리 처리할 작업을 여기서 진행합니다.
    switch(Type) {
        case EAA_LogIn:
            // DB에서 캐릭터 목록을 가져옵니다.
            break;
        case EAA_LogOut:
            // ClientSession 매니저에서 캐릭터를 관리합니다
            break;
        case EAA_SignUp: break;
        case EAA_SignOut: break;
        default:
            checkNoEntry();
    }
    AuthenticateResponse(Type, Result); // Send
}

FString UAccountManager::PasswordSHA256(const FString& In) {
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

FString UAccountManager::GetSalted(const FString& In) {
    // 시늉만
    return In + _T("_Unreal_Meta_Project_Account_Manager_Salt");
}




//AGenericGame* GameMode = Cast<AGenericGame>(GetWorld()->GetAuthGameMode());
//if (!GameMode) {
//    UE_LOG(LogTemp, Error, TEXT("게임모드를 로딩하는데 실패했습니다."));
//}

// TODO: 임시 로직입니다. 추후 Client Session 등으로 빼야 합니다.
// TODO: 컨트롤러 생성 후 캐릭터 만들어서 할당
// GetWorldSafe(this)->SpawnActor<AGenericController>(AGenericController::StaticClass());

//// TODO: 임시 로직입니다. 추후 Client Session 등으로 빼야 합니다.
//FActorSpawnParameters SpawnParams{};
//AActor* Player = GetWorld()->SpawnActor<AGenericCharacter>(FVector{ 100, 100, 100 }, FRotator{ 0, 0, 0 }, SpawnParams);
//if (!Player) {
//    UE_LOG(LogTemp, Error, TEXT("캐릭터 스폰 실패"));
//}
//else UE_LOG(LogTemp, Log, TEXT("캐릭터 스폰"));