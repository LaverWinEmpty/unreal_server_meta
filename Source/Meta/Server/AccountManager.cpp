#include "AccountManager.h"
#include "Server/DatabaseManager.h"
#include "Actor/GenericGame.h"
#include "Actor/GenericCharacter.h"
#include "Actor/GenericController.h"
#include "Misc/SecureHash.h"
#include "EncryptionContextOpenSSL.h"

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

void UAccountManager::Authenticate(EAuthReq Type, const FString& ID, const FString& PW) {
    UE_LOG(LogTemp, Log, TEXT("서버에 계정관리 요청을 보냅니다"));

    // server to client
    if (IsClient()) {
        UE_LOG(LogTemp, Log, TEXT("call UAccountManager::Authenticate() by Client"));
        AuthenticateRequest(static_cast<uint8>(Type), ID, PW); // client to server
        return;
    }

    // else is server
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
    AuthenticateRequest_Implementation(static_cast<uint8>(Type), ID, PW); // dierct call
}

void UAccountManager::AuthenticateRequest_Implementation(uint8 Type, const FString& ID, const FString& PW) {
    // TODO: DB에서 검사

    int32   Code;    // result code
    FString Message; // result message

    switch(static_cast<EAuthReq>(Type)) {
    case EAuthReq::EAR_LogIn:
        /* wrap */ {
            FString UserName = LogIn(ID, PW);
            Message          = GetResultMessage(0); // 임시 값
            if(UserName.IsEmpty()) {
                UE_LOG(LogTemp, Error, TEXT("Login: %s"), *Message);
            } else UE_LOG(LogTemp, Log, TEXT("Server: %s: %s"), *Message, *UserName);
        }
        break;

    case EAuthReq::EAR_LogOut:
        Code = LogOut(ID, PW);
        break;
    case EAuthReq::EAR_SignOut:
        Code = SignOut(ID, PW);
        break;
    case EAuthReq::EAR_SignUp:
        Code = SignUp(ID, PW);
        break;
    }

    if (static_cast<EAuthReq>(Type) != EAuthReq::EAR_LogIn) {
        Message = GetResultMessage(0); // 임시 값
        UE_LOG(LogTemp, Log, TEXT("Server: %s"), *Message);
    }

    // 응답처리
    if (IsClient()) {
        UE_LOG(LogTemp, Error, TEXT("응답 함수가 ClientMode에서 호출되었습니다."));
        check(false);
    }
    
    AuthenticateResponse(static_cast<uint8>(Type), ID, PW); // server to client
}


void UAccountManager::AuthenticateResponse_Implementation(uint8 Type, const FString& ID, const FString& PW) {
    UE_LOG(LogTemp, Log, TEXT("서버에서 클라이언트로 결과를 보냅니다"));
    if (IsDedicated()) {
        UE_LOG(LogTemp, Error, TEXT("응답 처리 함수가 DedicatedMode에서 호출되었습니다."));
        return;
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

}

FString UAccountManager::LogIn(const FString& ID, const FString& PW) {
    // TODO: DB에서 검사 후 유저 이름 반환
    return ID;
}

int32 UAccountManager::LogOut(const FString& ID, const FString& PW) {
    return 0;
}

int32 UAccountManager::SignUp(const FString& ID, const FString& PW) {
    FString HashedPW = HashSha256(PW);
    // INSERT
    UManager::Instance<UDatabaseManager>(this)->Query(
        "INSERT INTO user_tbl VALUES (?, ?)",
        [ID, HashedPW](sql::PreparedStatement* In) {
            In->setString(1, TCHAR_TO_UTF8(*ID));
            In->setString(2, TCHAR_TO_UTF8(*HashedPW));
        }
    );
    return 0;
}

int32 UAccountManager::SignOut(const FString& ID, const FString& PW) {
    // TODO: DB UPDATE -
    return 0;
}

FString UAccountManager::GetResultMessage(int Code) {
    // TODO:
    if(Code == 0) {
        return TEXT("Succeeded");
    }
    return TEXT("Failed");
}

FString UAccountManager::HashSha256(const FString& In) {
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
