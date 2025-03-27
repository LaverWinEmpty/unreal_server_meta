// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericController.generated.h"

/*********************************************************************************************************************
 * @brief 서버 기본 컨트롤러입니다.
 *********************************************************************************************************************/
UCLASS()
class META_API AGenericController : public APlayerController
{
	GENERATED_BODY()

public:
	AGenericController();
	
public:
	void BeginPlay() override;

public:
	UFUNCTION(Server, Reliable)
	void ConnectRequest(const FString& IP, int32 Port);
	void ConnectRequest_Implementation(const FString& IP, int32 Port);

public:
	UFUNCTION(NetMulticast, Unreliable)
	void InputResponse();
	void InputResponse_Implementation();
};
