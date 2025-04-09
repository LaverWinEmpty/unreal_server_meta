#pragma once

#include "CoreMinimal.h"
#include "ClientSession.generated.h"

USTRUCT(BlueprintType)
struct META_API FPlayerInfo {
    GENERATED_USTRUCT_BODY();

public:
    UPROPERTY() FString ID;
};