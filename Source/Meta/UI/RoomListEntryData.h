#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomListEntryData.generated.h"

UCLASS()
class META_API URoomListEntryData : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite) FString Name;
    UPROPERTY(BlueprintReadWrite) FString Address;
};
