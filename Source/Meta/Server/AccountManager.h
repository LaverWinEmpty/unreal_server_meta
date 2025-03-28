// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Manager/Manager.h"
#include "AccountManager.generated.h"

/*********************************************************************************************************************
 * 로그인 관련
 *********************************************************************************************************************/
UCLASS()
class META_API UAccountManager : public UManager {
    GENERATED_BODY()

public:
    enum class EAuthReq : uint8 {
        EAR_LogIn,   // 접속
        EAR_SignUp,  // 가입
        EAR_LogOut,  // 종료
        EAR_SignOut, // 탈퇴

        // 에러코드
    };

public:
    void Authenticate(EAuthReq In, const FString& ID, const FString& PW); // call by client -> process by server
protected:
    UFUNCTION(Server, Reliable)
    void AuthenticateRequest(uint8 In, const FString& ID, const FString& PW); // client request
protected:
    void AuthenticateRequest_Implementation(uint8 In, const FString& ID, const FString& PW);
protected:
    UFUNCTION(Client, Reliable)
    void AuthenticateResponse(uint8 In, const FString& ID, const FString& PW); // server response
protected:
    void AuthenticateResponse_Implementation(uint8 In, const FString& ID, const FString& PW);

protected:
    FString LogIn(  const FString& ID, const FString& PW); //!< 로그인
    int32   LogOut( const FString& ID, const FString& PW); //!< 로그아웃
    int32   SignUp( const FString& ID, const FString& PW); //!< 가입
    int32   SignOut(const FString& ID, const FString& PW); //!< 탈퇴

public:
    static FString GetResultMessage(int Code); //!< 임시 함수입니다

public:
    // FSHA256Hasher SHA256;

private:
    FString HashSha256(const FString&);
    static FString GetSalted(const FString&);
};

using enum UAccountManager::EAuthReq;
