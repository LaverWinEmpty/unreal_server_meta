#pragma once

#include "Enum/PlayerMeshEnum.h"
#include "PlayerOutfit.generated.h"

USTRUCT(BlueprintType)
struct META_API FPlayerOutfit {
    GENERATED_USTRUCT_BODY();

public:
    UPROPERTY() int BodyIndex;
    UPROPERTY() int OutfitIndex[EPO_OutfitCount];
};