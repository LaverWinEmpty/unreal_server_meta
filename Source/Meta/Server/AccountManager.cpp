#include "AccountManager.h"
#include "Actor/GenericGame.h"
#include "Actor/GenericCharacter.h"
#include "Actor/GenericController.h"

// TODO: 유틸로 만들기
// TMap<FString, FStringFormatArg> Args;
// Args.Emplace(TEXT("0"), Code);
// return FString::Format(TEXT("Error {0}"), Args);

void UAccountManager::Authenticate(EAuth Type, const FString& ID, const FString& PW) {
    UE_LOG(LogTemp, Log, TEXT("서버에 계정관리 요청을 보냅니다"));

    // server to client
    if (IsUser()) {
        UE_LOG(LogTemp, Log, TEXT("call UAccountManager::Authenticate() by Client"));
        AuthenticateRequest(static_cast<uint8>(Type), ID, PW); // client to server
    }

    if (IsHost()) {
        if (IsListen()) {
            UE_LOG(LogTemp, Log, TEXT("call UAccountManager::Authenticate() by Listen"));
        }
        else if (IsStandalone()) {
            UE_LOG(LogTemp, Log, TEXT("call UAccountManager::Authenticate() by Standalone"));
        }
        else checkNoEntry();
        AuthenticateRequest_Implementation(Type, ID, PW); // dierct call
    }

    else {
        UE_LOG(LogTemp, Error, TEXT("Request call by Dedicated Server."));
        check(false);
    }
}

void UAccountManager::AuthenticateRequest_Implementation(uint8 Type, const FString& ID, const FString& PW) {
    // TODO: DB에서 검사

    int32   Code;    // result code
    FString Message; // result message

    switch(static_cast<EAuth>(Type)) {
    case EAuth::A_LogIn:
        /* wrap */ {
            FString UserName = LogIn(ID, PW);
            Message          = GetResultMessage(0); // 임시 값
            if(UserName.IsEmpty()) {
                UE_LOG(LogTemp, Error, TEXT("Login: %s"), *Message);
            } else UE_LOG(LogTemp, Log, TEXT("Server: %s: %s"), *Message, *UserName);
        }
        break;

    case EAuth::A_LogOut:
        Code = LogOut(ID, PW);
        break;
    case EAuth::A_SignOut:
        Code = SignOut(ID, PW);
        break;
    case EAuth::A_SignIn:
        Code = SignOut(ID, PW);
        break;
    }

    if (static_cast<EAuth>(Type) != EAuth::A_LogIn) {
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

    // TODO: 임시 로직입니다. 추후 Client Session 등으로 빼야 합니다.
    FActorSpawnParameters SpawnParams{};
    AActor* Player = GetWorld()->SpawnActor<AGenericCharacter>(FVector{ 100, 100, 100 }, FRotator{ 0, 0, 0 }, SpawnParams);
    if (!Player) {
        UE_LOG(LogTemp, Error, TEXT("캐릭터 스폰 실패"));
    }
    else UE_LOG(LogTemp, Log, TEXT("캐릭터 스폰"));

}

FString UAccountManager::LogIn(const FString& ID, const FString& PW) {
    // TODO: DB에서 검사 후 유저 이름 반환
    return ID;
}

int32 UAccountManager::LogOut(const FString& ID, const FString& PW) {
    // TODO: Client Session 검색
    return 0;
}

int32 UAccountManager::SignUp(const FString& ID, const FString& PW) {
    // TODO: DB INSERT -
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
