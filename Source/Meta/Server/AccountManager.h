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
    enum EAuthAction : int8 {
        EAA_LogIn,   // 접속
        EAA_LogOut,  // 종료
        EAA_SignUp,  // 가입
        EAA_SignOut, // 탈퇴
    };

public:
    enum EAuthResult : int8 {
        EAR_Uknown = -1,
        EAR_Suceeded,     // 성공
        EAR_AlreadyExist, // 이미 존재하는 계정
        EAR_NotExist,     // 존재하지 않는 계정
    };

public:
    //! @brief 코드를 문자열로 변환합니다
    static FString ConvertActionTypeName(int8);

    //! @brief 코드를 문자열로 변환합니다
    static FString ConvertActionTypeName(EAuthAction);

public:
    //! @brief 결과값 메세지로 가지고 옵니다
    static FString GetResultMessage(int8);

    //! @brief 결과값 메세지로 가지고 옵니다
    static FString GetResultMessage(EAuthResult);

public:
    void Authenticate(EAuthAction In, const FString& ID, const FString& PW); // call by client -> process by server
protected:
    UFUNCTION(Server, Reliable)
    void AuthenticateRequest(int8 Type, const FString& ID, const FString& PW); // client request
protected:
    void AuthenticateRequest_Implementation(int8 Type, const FString& ID, const FString& PW);
protected:
    UFUNCTION(Client, Reliable)
    void AuthenticateResponse(int8 Type, int8 Result); // server response
protected:
    void AuthenticateResponse_Implementation(int8 Type, int8 Result);

protected:
    void LogIn(  const FString& ID, const FString& PW); //!< 로그인
    void LogOut( const FString& ID, const FString& PW); //!< 로그아웃
    void SignUp( const FString& ID, const FString& PW); //!< 가입
    void SignOut(const FString& ID, const FString& PW); //!< 탈퇴

private:
    void PostAuthenticate(int8 Type, int8 Result);

private:
    FString PasswordSHA256(const FString&);
    static FString GetSalted(const FString&);
};

using EAuthAction = UAccountManager::EAuthAction;
using EAuthResult = UAccountManager::EAuthResult;
using enum EAuthAction;
using enum EAuthResult;