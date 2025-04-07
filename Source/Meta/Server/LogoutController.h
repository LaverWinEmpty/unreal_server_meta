#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LogoutController.generated.h"

UCLASS()
class META_API ALogoutController : public APlayerController
{
	GENERATED_BODY()

public:
	ALogoutController();

public:
	void BeginPlay() override;

//public:
//	UFUNCTION(Server, Reliable)
//	void LogoutRequest();
//	UFUNCTION(Client, Reliable)
//	void LogoutResponse();
//
//private:
//	void LogoutRequest_Implementation();
//	void LogoutResponse_Implementation();
};
