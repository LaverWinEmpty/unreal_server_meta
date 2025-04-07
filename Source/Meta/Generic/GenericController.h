// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "GameFramework/PlayerController.h"
#include "Server/LogoutController.h"
#include "GenericController.generated.h"

/*********************************************************************************************************************
 * @brief 범용 컨트롤러입니다, 서버 클라이언트 모두 사용
 *********************************************************************************************************************/
UCLASS()
class META_API AGenericController: public ALogoutController {
    GENERATED_BODY()

public:
    AGenericController();

public:
    void BeginPlay() override;

public:
    void OnLogOut(int8 Type, int8 Result); // response
};
