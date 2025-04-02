#include "Client/LoginController.h"
#include "Server/AccountManager.h"

#include "UI/LoginUI.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

ALoginController::ALoginController() {
    // duplicated 오류 수정해야됨
    static ConstructorHelpers::FClassFinder<UUserWidget> Finder(TEXT("/Game/Assets/UI/BP_LoginUI"));
    if (Finder.Succeeded()) {
        LoginWidgetClass = Finder.Class;
    }

    PrimaryActorTick.bCanEverTick = true;
    bReplicates                   = true;
}

void ALoginController::BeginPlay() {
    Super::BeginPlay();

    // 마우스 보이게
    GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;

    // 로그인 위젯 생성
    Widget = CreateWidget<ULoginUI>(GetWorld(), LoginWidgetClass);
    if (!Widget) {
        check(false);
    }
    Widget->AddToViewport();

    // 로그인 버튼 바인드
    if (Widget->LogInButton) {
        Widget->LogInButton->OnClicked.AddDynamic(this, &ALoginController::OnLogIn);
        Widget->SignUpButton->OnClicked.AddDynamic(this, &ALoginController::OnSignUp);
    }
}

void ALoginController::ConnectRequest_Implementation(const FString& IP, int32 Port) {}

void ALoginController::InputResponse_Implementation() {}

// 응답 받고 처리되는 함수입니다.
void ALoginController::OnAuthenticate(uint8 Type, uint8 Result) {
    // has error
    if (Result) {
        Widget->ResultText->SetColorAndOpacity(FColor::Red);
    }

    // 로그인의 경우레벨 변경됨
    else if(Type != EAA_LogIn){
        Widget->ResultText->SetColorAndOpacity(FColor::White);
    }

    // 결과 출력
    Widget->ResultText->SetText(
        FText::FromString(UAccountManager::GetResultMessage(Result))
    );
}

void ALoginController::OnLogIn() {
    FString ID = Widget->InputID->GetText().ToString();
    FString PW = Widget->InputPW->GetText().ToString();
    UAccountManager::Instance(this)->Authenticate(EAA_LogIn, ID, PW);
}

void ALoginController::OnSignUp() {
    FString ID = Widget->InputID->GetText().ToString();
    FString PW = Widget->InputPW->GetText().ToString();
    UAccountManager::Instance(this)->Authenticate(EAA_SignUp, ID, PW);
}

void ALoginController::OnLogOut() {
}

void ALoginController::OnCreateCharacter() {
}

void ALoginController::OnCreateRoom() {
}

void ALoginController::OnEnterRoom() {
}
