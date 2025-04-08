// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum/AuthEnum.h"
#include "Enum/ResultEnum.h"
#include "GameFramework/PlayerController.h"
#include "LoginController.generated.h"

/*********************************************************************************************************************
 * @brief 클라이언트의 로그인 관련 컨트롤러입니다.
 *********************************************************************************************************************/

class ULoginUI;

UCLASS()
class META_API ALoginController: public APlayerController {
    GENERATED_BODY()

public:
    ALoginController();

public:
    void BeginPlay() override;

public:
    UFUNCTION() void OnLogIn();  // Login UI button
    UFUNCTION() void OnSignUp(); // Login UI button

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
    void LogIn(const FString& ID, const FString& PW); //!< 로그인
    void LogOut(const FString& ID, const FString& PW); //!< 로그아웃
    void SignUp(const FString& ID, const FString& PW); //!< 가입
    void SignOut(const FString& ID, const FString& PW); //!< 탈퇴

private:
    void PostAuthenticate(const FString& ID, int8 Type, int8 Result);
    
private:
    FString PasswordSHA256(const FString&);
    FString GetSalted(const FString&);

private:
    TSubclassOf<UUserWidget> LoginWidgetClass;

private:
    ULoginUI* Widget;
};