// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameIntroHandler.generated.h"

/*********************************************************************************************************************
 * @brief 클라이언트 시작 후 네트워크에 접속을 돕는 클래스입니다.
 *********************************************************************************************************************/
// TODO: standalone일때 따로 처리해주기

class ULoginUI;

UCLASS()
class META_API AGameIntroHandler : public APlayerController {
    GENERATED_BODY()

private:
    static const FString ServerIP;

public:
    AGameIntroHandler();

public:
    void BeginPlay() override;

};