#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Data/PlayerPreset.h"
#include "LobbyHandler.generated.h"

/**************************************************************************************************
 * Widget
 **************************************************************************************************/
class UUserWidget;
class ULoginUI;
class ULobbyUI;
class UCharacterCustomizeUI;
class UMessageBoxUI;

/**************************************************************************************************
 * Player Actor
 **************************************************************************************************/
class USkeletalMesh;
class ALobbyActor;

/**************************************************************************************************
 * Class
 **************************************************************************************************/
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
    // RPC response wrapping
    void EnterLobbyModeResponse();
    void EnterLoginModeResponse();
    void EnterCustomizeModeResponse();
    void GetResultMessageResponse(int8);
    void NewCharacterResponse(const FPlayerPreset&);
    void LoadCharactersResponse(const TArray<FPlayerPreset>&);

public:
    static FString PasswordSHA256(const FString&);
    static FString GetSalted(const FString&);

public:
    void           Initialize(); // UI init
    void           NextOutfit(int OutfitIndex);
    void           PrevOutfit(int OutfitIndex);
    void           BodySelect(int, bool = false); //!< false: outfit init, else keep
    USkeletalMesh* GetSelectedBodyMesh() const;
    USkeletalMesh* GetSelectedOutfitMesh(int) const;
    void           AddCharacterToList(const FPlayerPreset&);
    void           SelectCharacterFromList(int32);
    void           LoadCharacterList(const FString& ID);

public:
    UFUNCTION() void OnLogIn();   //!< Login UI LogInButton
    UFUNCTION() void OnSignUp();  //!< Login UI SignUpButton
    UFUNCTION() void OnLogOut();  //!< Login UI LogOutButton
    UFUNCTION() void OnSignOut(); //!< Login UI SignOutButton

public:
    UFUNCTION() void OnBodyNext();                            //!< Character Customize UI Body NextButton
    UFUNCTION() void OnBodyPrev();                            //!< Character Customize UI Body PrevButton
    UFUNCTION() void OnFaceNext() { NextOutfit(EPL_Face); }   //!< Character Customize UI Face NextButton
    UFUNCTION() void OnFacePrev() { PrevOutfit(EPL_Face); }   //!< Character Customize UI Face PrevButton
    UFUNCTION() void OnHairNext() { NextOutfit(EPL_Hair); }   //!< Character Customize UI Hair NextButton
    UFUNCTION() void OnHairPrev() { PrevOutfit(EPL_Hair); }   //!< Character Customize UI Hair PrevButton
    UFUNCTION() void OnUpperNext() { NextOutfit(EPL_Upper); } //!< Character Customize UI Upper NextButton
    UFUNCTION() void OnUpperPrev() { PrevOutfit(EPL_Upper); } //!< Character Customize UI Upper PrevButton
    UFUNCTION() void OnLowerNext() { NextOutfit(EPL_Lower); } //!< Character Customize UI Lower NextButton
    UFUNCTION() void OnLowerPrev() { PrevOutfit(EPL_Lower); } //!< Character Customize UI Lower PrevButton
    UFUNCTION() void OnShoesNext() { NextOutfit(EPL_Shoes); } //!< Character Customize UI Shoes NextButton
    UFUNCTION() void OnShoesPrev() { PrevOutfit(EPL_Shoes); } //!< Character Customize UI Shoes PrevButton

public:
    UFUNCTION() void OnCustomBegin();  //!< Character Customize UI CustomBegin
    UFUNCTION() void OnCustomEnd();    //!< Character Customize UI CustomEnd
    UFUNCTION() void OnCustomCancel(); //!< Character Customize UI CustomCancel
    UFUNCTION() void OnStart();        //!< Character Customize UI StartButton

public:
    UFUNCTION() void ShowMessageBox();
    UFUNCTION() void HideMessageBox();

public:
    UFUNCTION(Client, Reliable) void EnterLoginModeToClient();
    UFUNCTION(Client, Reliable) void EnterLobbyModeToClient();
    UFUNCTION(Client, Reliable) void EnterCustomizeModeToClient();
private:
    void EnterLoginModeToClient_Implementation();
    void EnterLobbyModeToClient_Implementation();
    void EnterCustomizeModeToClient_Implementation();

protected:
    UFUNCTION(Server, Reliable) void LogInToServer(const FString& ID, const FString& PW);
    UFUNCTION(Server, Reliable) void LogOutToServer();
    UFUNCTION(Server, Reliable) void SignUpToServer(const FString& ID, const FString& PW);
    UFUNCTION(Server, Reliable) void SignOutToServer(const FString& ID, const FString& PW);
private:
    void LogInToServer_Implementation(const FString& ID, const FString& PW);
    void LogOutToServer_Implementation();
    void SignUpToServer_Implementation(const FString& ID, const FString& PW);
    void SignOutToServer_Implementation(const FString& ID, const FString& PW);

public:
    UFUNCTION(Server, Reliable) void NewCharacterToServer(const FPlayerPreset& Outfit);
    UFUNCTION(Client, Reliable) void NewCharacterToClient(const FPlayerPreset& Outfit);
    UFUNCTION(Client, Reliable) void LoadCharactersToClient(const TArray<FPlayerPreset>& Param);
private:
    void NewCharacterToServer_Implementation(const FPlayerPreset& Outfit);
    void NewCharacterToClient_Implementation(const FPlayerPreset& Outfit);
    void LoadCharactersToClient_Implementation(const TArray<FPlayerPreset>& Param);

public:
    UFUNCTION(Client, Reliable)void GetResultMessageToClient(int8 Code);
private:
    void GetResultMessageToClient_Implementation(int8 Code);

public:
    ULoginUI*              LoginUI;
    ULobbyUI*              LobbyUI;
    UCharacterCustomizeUI* CharacterCustomizeUI;
    UMessageBoxUI*         MessageBoxUI;

private:
    FPlayerPreset Selected    = { 0 }; //!< 현재 고른 캐릭터 정보
    int32         SelectIndex = 0;     //!< 캐릭터 리스트 뷰에서 선택한 번호
    int32         SelectMax   = 0;     //!< 캐릭터 선택 가능 수

private:
    int OutfitItemCount[EPB_Count][EPL_Count] = { 0 }; //!< 각 Body의 Outfit별 아이템 가짓수

private:
    ALobbyActor*  Actor;
    ACameraActor* Viewer;

private:
    TSubclassOf<UUserWidget> LoginUiClass;
    TSubclassOf<UUserWidget> LobbyUiClass;
    TSubclassOf<UUserWidget> CharacterCustomizeUiClass;
    TSubclassOf<UUserWidget> MessageBoxUiClass;
};