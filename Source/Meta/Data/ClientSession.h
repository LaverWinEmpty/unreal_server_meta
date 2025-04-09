#pragma once

#include "CoreMinimal.h"
#include "ClientSession.generated.h"

USTRUCT(BlueprintType)
struct META_API FClientSession {
    GENERATED_BODY();

public:
    FClientSession() = default;
    FClientSession(const FString&);

public:
    UPROPERTY() FString ID;
};