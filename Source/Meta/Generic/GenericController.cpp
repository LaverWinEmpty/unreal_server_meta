#include "Generic/GenericController.h"
#include "Generic/GenericCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Manager/Manager.h"

#include "EnhancedActionKeyMapping.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "InputTriggers.h"
#include "Generic/GenericController.inl"

void AGenericController::Mapping(int InputActionID, const FKey Key) {
	IMC->MapKey(IA[InputActionID], Key);
}

AGenericController::AGenericController() {
    PrimaryActorTick.bCanEverTick = true;
    bReplicates                   = true;
}

void AGenericController::BeginPlay() {
    Super::BeginPlay();

	// InputMappicContext 생성
	// InputAction 생성
	// InputAction 유형 세팅
	if (UManager::IsUser(this)) {
		IMC = NewObject<UInputMappingContext>();
		check(IMC);

		IA.Init(nullptr, EIA_Count);
		for (int i = 0; i < EIA_Count; ++i) {
			IA[i] = NewObject<UInputAction>();
			check(IA[i]);
		}

		// Set
		IA[EIA_Jump]->ValueType = EInputActionValueType::Boolean;
		IA[EIA_Move]->ValueType = EInputActionValueType::Axis2D;
		IA[EIA_Look]->ValueType = EInputActionValueType::Axis2D;

		// Init
		AGenericCharacter* GotPawn = Cast<AGenericCharacter>(GetPawn());
		check(GotPawn);

		ULocalPlayer* LP = GetLocalPlayer();
		check(LP);

		UEnhancedInputLocalPlayerSubsystem* EI = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		check(EI);

		UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
		check(EIC);

		EI->AddMappingContext(IMC, 0); // maaping IMC

		EIC->BindAction(IA[EIA_Jump], ETriggerEvent::Started,   GotPawn, &AGenericCharacter::Jump);
		EIC->BindAction(IA[EIA_Move], ETriggerEvent::Triggered, GotPawn, &AGenericCharacter::Move);

		SetUpKeyMapping();
	}
}

void AGenericController::OnPossess(APawn* In) {
	Super::OnPossess(In);
		// DB에서 가져와서 어떻게할까
	if (UManager::IsServer(this)) {
	}
}

void AGenericController::SetUpKeyMapping() {
	//TODO: json 등 설정 파일에서 가져오도록 수정해야 합니다

	Mapping(EIA_Jump, EKeys::SpaceBar);
	Mapping(EIA_Move, EKeys::W);
	Mapping<UInputModifierNegate>(EIA_Move, EKeys::S);
	Mapping<UInputModifierSwizzleAxis>(EIA_Move, EKeys::D);
	Mapping<UInputModifierSwizzleAxis, UInputModifierNegate>(EIA_Move, EKeys::A);
}

//// 응답 받고 처리되는 함수입니다.
//void AGenericController::PostLogout(int8 Type, int8 Result) {
//    
//}