// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
    UFUNCTION() void OnLogIn();           // Login UI button
    UFUNCTION() void OnSignUp();          // Login UI button
    UFUNCTION() void OnCreateCharacter(); // Lobby UI button
    UFUNCTION() void OnCreateRoom();      // Lobby UI button
    UFUNCTION() void OnEnterRoom();       // Lobby UI button

public:
    void OnAuthenticate(int8, int8);

private:
    TSubclassOf<UUserWidget> LoginWidgetClass;

private:
    ULoginUI* Widget;
};