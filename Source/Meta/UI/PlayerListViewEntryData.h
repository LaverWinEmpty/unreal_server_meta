#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerListViewEntryData.generated.h"

UCLASS()
class META_API UPlayerListViewEntryData: public UObject {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite) FString Name;
    UPROPERTY(BlueprintReadWrite) int32 BodyIndex;
    UPROPERTY(BlueprintReadWrite) int32 FaceIndex;
    UPROPERTY(BlueprintReadWrite) int32 HairIndex;
    UPROPERTY(BlueprintReadWrite) int32 UpperIndex;
    UPROPERTY(BlueprintReadWrite) int32 LowerIndex;
    UPROPERTY(BlueprintReadWrite) int32 ShoesIndex;
};
