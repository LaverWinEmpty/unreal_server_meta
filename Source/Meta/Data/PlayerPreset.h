#pragma once

#include "CoreMinimal.h"
#include "Enum/PlayerMeshEnum.h"
#include "PlayerPreset.generated.h"

USTRUCT(BlueprintType)
struct META_API FPlayerPreset {
    GENERATED_USTRUCT_BODY();

public:
    UPROPERTY() int     BodyCode;
    UPROPERTY() int     LookCode[EPL_Count];
    UPROPERTY() FString Name;
};