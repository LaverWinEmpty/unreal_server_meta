#pragma once

/*********************************************************************************************************************
 * @brief 클라이언트 시작 모드입니다.
 *********************************************************************************************************************/

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameIntro.generated.h"

UCLASS()
class META_API AGameIntro : public AGameMode
{
	GENERATED_BODY()

public:
	AGameIntro();
};
