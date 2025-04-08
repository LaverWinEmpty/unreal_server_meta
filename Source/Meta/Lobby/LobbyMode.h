#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyMode.generated.h"

class ULoginUI;
class ULobbyUI;
class UCharacterCustomizeUI;

/*********************************************************************************************************************
 * @brief 로비 모드입니다.
 *********************************************************************************************************************/
UCLASS()
class META_API ALobbyMode: public AGameMode {
    GENERATED_BODY()

public:
    ALobbyMode();

protected:
    virtual void BeginPlay() override;
};
