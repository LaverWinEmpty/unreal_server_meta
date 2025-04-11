#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterListEntryData.generated.h"

UCLASS()
class META_API UCharacterListEntryData: public UObject {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite) int32 Index; //!< character order

public:
    UPROPERTY(BlueprintReadWrite) FString Name;
    UPROPERTY(BlueprintReadWrite) int32 BodyCode;
    UPROPERTY(BlueprintReadWrite) int32 FaceCode;
    UPROPERTY(BlueprintReadWrite) int32 HairCode;
    UPROPERTY(BlueprintReadWrite) int32 UpperCode;
    UPROPERTY(BlueprintReadWrite) int32 LowerCode;
    UPROPERTY(BlueprintReadWrite) int32 ShoesCode;
};
