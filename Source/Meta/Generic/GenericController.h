#pragma once
#include "CoreMinimal.h"
#include "Enum/InputEnum.h"
#include "GameFramework/PlayerController.h"
#include "Server/LogoutController.h"
#include "GenericController.generated.h"

struct FEnhancedActionKeyMapping;
class UInputMappingContext;
class UInputAction;

/**************************************************************************************************
 * @brief 범용 컨트롤러
 **************************************************************************************************/
UCLASS()
class META_API AGenericController: public APlayerController {
    GENERATED_BODY()

private:
	template<typename Arg, typename... Args> void PostMapping(FEnhancedActionKeyMapping* Out);

public:
	//! @tparam UInputModifier, UInputTrigger
	template<typename Arg, typename... Args> void Mapping(int InputActionID, const FKey Key);
	//! @tparam UInputModifier, UInputTrigger
	void Mapping(int InputActionID, const FKey Key);

public:
    AGenericController();

public:
	void OnPossess(APawn*) override;
    void BeginPlay() override;

public:
	void SetUpKeyMapping();

public:
    // void PostLogout(int8 Type, int8 Result); // response

public:
	TObjectPtr<UInputMappingContext> IMC;
	TArray<TObjectPtr<UInputAction>> IA;
};