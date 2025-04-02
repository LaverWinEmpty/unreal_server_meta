#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Login.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class ULogin: public UInterface {
    GENERATED_BODY()
};

/* 실제 인터페이스 선언. */
class ILogin {
    GENERATED_BODY()
public:
};
