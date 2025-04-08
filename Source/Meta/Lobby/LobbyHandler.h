#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Data/PlayerInfo.h"
#include "LobbyHandler.generated.h"

/******************************************************************************
 * Widget
 ******************************************************************************/
class UUserWidget;
class ULoginUI;
class ULobbyUI;
class UCharacterCustomizeUI;
class UMessageBoxUI;

/******************************************************************************
 * Player Actor
 ******************************************************************************/
class USkeletalMesh;
class ACustomizePreviewActor;

/******************************************************************************
 * Class
 ******************************************************************************/
UCLASS() class META_API ALobbyHandler: public APlayerController {
    GENERATED_BODY()

public:
    ALobbyHandler();

public:
    virtual void BeginPlay() override;

public:
    void BeginClient();
    void BeginServer();

public:
    // Client Server 함수 wrapping한거
    void ToLobbyUI();
    void ToLoginUI();
    void ToCustomUI();

//public:
//    void TryLogin();
//    void TryLogout();
//    void TrySignup();
//    void TrySignout();

public:
    static FString PasswordSHA256(const FString&);
    static FString GetSalted(const FString&);

public:
    void           NextOutfit(int OutfitIndex);
    void           PrevOutfit(int OutfitIndex);
    void           BodySelect(int, bool = false); //!< false: outfit init, else keep
    USkeletalMesh* GetSelectedBodyMesh() const;
    USkeletalMesh* GetSelectedOutfitMesh(int) const;
    void           SetPreviewCharacter(int32); //!< from list view
    void           AddListView(const FString&, const FPlayerOutfit&);
    void           LoadCharacterList(const FString& ID);

public:
    UFUNCTION() void OnLogIn();   //!< Login UI LogInButton
    UFUNCTION() void OnSignUp();  //!< Login UI SignUpButton
    UFUNCTION() void OnLogOut();  //!< Login UI LogOutButton
    UFUNCTION() void OnSignOut(); //!< Login UI SignOutButton

public:
    UFUNCTION() void OnBodyNext();                            //!< Character Customize UI Body NextButton
    UFUNCTION() void OnBodyPrev();                            //!< Character Customize UI Body PrevButton
    UFUNCTION() void OnFaceNext() { NextOutfit(EPO_Face); }   //!< Character Customize UI Face NextButton
    UFUNCTION() void OnFacePrev() { PrevOutfit(EPO_Face); }   //!< Character Customize UI Face PrevButton
    UFUNCTION() void OnHairNext() { NextOutfit(EPO_Hair); }   //!< Character Customize UI Hair NextButton
    UFUNCTION() void OnHairPrev() { PrevOutfit(EPO_Hair); }   //!< Character Customize UI Hair PrevButton
    UFUNCTION() void OnUpperNext() { NextOutfit(EPO_Upper); } //!< Character Customize UI Upper NextButton
    UFUNCTION() void OnUpperPrev() { PrevOutfit(EPO_Upper); } //!< Character Customize UI Upper PrevButton
    UFUNCTION() void OnLowerNext() { NextOutfit(EPO_Lower); } //!< Character Customize UI Lower NextButton
    UFUNCTION() void OnLowerPrev() { PrevOutfit(EPO_Lower); } //!< Character Customize UI Lower PrevButton
    UFUNCTION() void OnShoesNext() { NextOutfit(EPO_Shoes); } //!< Character Customize UI Shoes NextButton
    UFUNCTION() void OnShoesPrev() { PrevOutfit(EPO_Shoes); } //!< Character Customize UI Shoes PrevButton

public:
    // 이벤트
    UFUNCTION() void OnCustomBegin();  //!< Character Customize UI CustomBegin
    UFUNCTION() void OnCustomEnd();    //!< Character Customize UI CustomEnd
    UFUNCTION() void OnCustomCancel(); //!< Character Customize UI CustomCancel
    UFUNCTION() void OnStart();        //!< Character Customize UI StartButton

public:
    UFUNCTION(Client, Reliable) void LoginModeResponse();
    UFUNCTION(Client, Reliable) void LobbyModeResponse();
    UFUNCTION(Client, Reliable) void CustomModeResponse();
private:
    void LoginModeResponse_Implementation();
    void LobbyModeResponse_Implementation();
    void CustomModeResponse_Implementation();

protected:
    UFUNCTION(Server, Reliable) void LogInRequest(const FString& ID, const FString& PW);
    UFUNCTION(Server, Reliable) void LogOutRequest(const FString& ID);
    UFUNCTION(Server, Reliable) void SignUpRequest(const FString& ID, const FString& PW);
    UFUNCTION(Server, Reliable) void SignOutRequest(const FString& ID, const FString& PW);
private:
    void LogInRequest_Implementation(const FString& ID, const FString& PW);
    void LogOutRequest_Implementation(const FString& ID);
    void SignUpRequest_Implementation(const FString& ID, const FString& PW);
    void SignOutRequest_Implementation(const FString& ID, const FString& PW);

public:
    UFUNCTION(Server, Reliable) void CreateCharacterRequest(const FString& Name, const FPlayerOutfit& Outfit);
    UFUNCTION(Client, Reliable) void CreateCharacterResponse(const FString& Name, const FPlayerOutfit& Outfit);
    UFUNCTION(Client, Reliable) void LoadCharacterListResponse(const TArray<FPlayerInfo>& Param);
private:
    void CreateCharacterRequest_Implementation(const FString& Name, const FPlayerOutfit& Outfit);
    void CreateCharacterResponse_Implementation(const FString& Name, const FPlayerOutfit& Outfit);
    void LoadCharacterListResponse_Implementation(const TArray<FPlayerInfo>& Param);

public:
    UFUNCTION(Client, Reliable) void ResultResponse(int8 Code);
private:
    void ResultResponse_Implementation(int8 Code);

public:
    ULoginUI*              LoginUI;
    ULobbyUI*              LobbyUI;
    UCharacterCustomizeUI* CharacterCustomizeUI;
    UMessageBoxUI*         UMessageBoxUI;


private:
    FPlayerOutfit Selected                            = { 0 }; //!< 현재 고른 캐릭터 정보
    int32           SelectIndex                         = 0;     //!< 캐릭터 리스트 뷰에서 선택한 번호
    int32           SelectMax                           = 0;     //!< 캐릭터 선택 가능 수
    int OutfitItemCount[EPB_BodyCount][EPO_OutfitCount] = { 0 }; //!< 각 Body의 Outfit별 아이템 가짓수

private:
    ACustomizePreviewActor* Actor;
    ACameraActor*           Viewer;

private:
    TSubclassOf<UUserWidget> LoginUiClass;
    TSubclassOf<UUserWidget> LobbyUiClass;
    TSubclassOf<UUserWidget> CharacterCustomizeUiClass;
    TSubclassOf<UUserWidget> MessageBoxUiClass;
};