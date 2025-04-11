#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Data/PlayerPreset.h"
#include "LobbyHandler.generated.h"

/**************************************************************************************************
 * Widget
 **************************************************************************************************/
class UUserWidget;
class UMessageBoxUI;
class UInputBoxUI;
class ULoginUI;
class ULobbyUI;
class UCharacterCustomizeUI;

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
    static FString PasswordSHA256(const FString&);
    static FString GetSalted(const FString&);

public:
    void Initialize();                           //!< UI init in client
    void NextLook(int LookType);                 //!< select next look
    void PrevLook(int LookType);                 //!< select prev look
    void BodySelect(int BodyType, bool = false); //!< select body in client, true: keep selected look index
    void SelectCharacterFromList(int32 Index);   //!< select character in client

public:
    USkeletalMesh* GetSelectedBodyMesh() const;    //!< get selected body mesh in client
    USkeletalMesh* GetSelectedLookMesh(int) const; //!< get selected look mesh in client

protected:
    void PostNewCharacter(const FPlayerPreset&); //!< On new character succeeded in client
    void PostLogIn(const FString& ID);           //!< On log-in succedded in server

public:
    UFUNCTION() void OnLogIn();   //!< Login UI LogInButton
    UFUNCTION() void OnSignUp();  //!< Login UI SignUpButton
    UFUNCTION() void OnLogOut();  //!< Login UI LogOutButton
    UFUNCTION() void OnSignOut(); //!< Login UI SignOutButton

public:
    UFUNCTION() void OnBodyNext();                            //!< Character Customize UI Body NextButton
    UFUNCTION() void OnBodyPrev();                            //!< Character Customize UI Body PrevButton
    UFUNCTION() void OnFaceNext() { NextLook(EPL_Face); }   //!< Character Customize UI Face NextButton
    UFUNCTION() void OnFacePrev() { PrevLook(EPL_Face); }   //!< Character Customize UI Face PrevButton
    UFUNCTION() void OnHairNext() { NextLook(EPL_Hair); }   //!< Character Customize UI Hair NextButton
    UFUNCTION() void OnHairPrev() { PrevLook(EPL_Hair); }   //!< Character Customize UI Hair PrevButton
    UFUNCTION() void OnUpperNext() { NextLook(EPL_Upper); } //!< Character Customize UI Upper NextButton
    UFUNCTION() void OnUpperPrev() { PrevLook(EPL_Upper); } //!< Character Customize UI Upper PrevButton
    UFUNCTION() void OnLowerNext() { NextLook(EPL_Lower); } //!< Character Customize UI Lower NextButton
    UFUNCTION() void OnLowerPrev() { PrevLook(EPL_Lower); } //!< Character Customize UI Lower PrevButton
    UFUNCTION() void OnShoesNext() { NextLook(EPL_Shoes); } //!< Character Customize UI Shoes NextButton
    UFUNCTION() void OnShoesPrev() { PrevLook(EPL_Shoes); } //!< Character Customize UI Shoes PrevButton

public:
    UFUNCTION() void OnCustomBegin();      //!< Character Customize UI CustomBegin
    UFUNCTION() void OnCustomEnd();        //!< Character Customize UI CustomEnd
    UFUNCTION() void OnCustomCancel();     //!< Character Customize UI CustomCancel
    UFUNCTION() void OnStart();            //!< Lobby UI StartButton
    UFUNCTION() void OnShowUICreateRoom(); //!< Lobby UI CreateRoomButton
    UFUNCTION() void OnCreateRoom();       //!< CreateRoom UI

protected:
    UFUNCTION(Client, Reliable)
    void EnterLoginModeToClient();
    void EnterLoginModeToClient_Implementation();
public:
    void EnterLoginModeResponse();

protected:
    UFUNCTION(Client, Reliable)
    void EnterLobbyModeToClient();
    void EnterLobbyModeToClient_Implementation();
public:
    void EnterLobbyModeResponse();

protected:
    UFUNCTION(Client, Reliable)
    void EnterCustomizeModeToClient();
    void EnterCustomizeModeToClient_Implementation();
public:
    void EnterCustomizeModeResponse();

protected:
    UFUNCTION(Server, Reliable)
    void LogInToServer(const FString& ID, const FString& PW);
    void LogInToServer_Implementation(const FString& ID, const FString& PW);
public:
    void LogInRequest(const FString& ID, const FString& PW);

protected:
    UFUNCTION(Server, Reliable)
    void LogOutToServer();
    void LogOutToServer_Implementation();
public:
    void LogOutRequest();

protected:
    UFUNCTION(Server, Reliable, WithValidation)
    void SignUpToServer(const FString& ID, const FString& PW);
    void SignUpToServer_Implementation(const FString& ID, const FString& PW);
    bool SignUpToServer_Validate(const FString& ID, const FString& PW);
public:
    void SignUpRequest(const FString& ID, const FString& PW);

protected:
    UFUNCTION(Server, Reliable)
    void SignOutToServer(const FString& ID, const FString& PW);
    void SignOutToServer_Implementation(const FString& ID, const FString& PW);
public:
    void SignOutRequest(const FString& ID, const FString& PW);

protected:
    UFUNCTION(Server, Reliable, WithValidation)
    void NewCharacterToServer(const FPlayerPreset& Param);
    void NewCharacterToServer_Implementation(const FPlayerPreset& Param);
    bool NewCharacterToServer_Validate(const FPlayerPreset& Prama);
public:
    void NewCharacterRequest(const FPlayerPreset& Param);

protected:
    UFUNCTION(Client, Reliable)
    void NewCharacterToClient(const FPlayerPreset& Param);
    void NewCharacterToClient_Implementation(const FPlayerPreset& Param);
public:
    void NewCharacterResponse(const FPlayerPreset& Param);

protected:
    UFUNCTION(Client, Reliable)
    void LoadCharactersToClient(const TArray<FPlayerPreset>& Param);
    void LoadCharactersToClient_Implementation(const TArray<FPlayerPreset>& Param);
public:
    void LoadCharactersResponse(const TArray<FPlayerPreset>& Param);

protected:
    UFUNCTION(Client, Reliable)
    void PrintResultMessageToClient(int8 Code);
    void PrintResultMessageToClient_Implementation(int8 Code);
public:
    void PrintResultMessageResponse(int8);

protected:
    UFUNCTION(Server, Reliable)
    void CreateRoomToServer(const FString& Name, const FString& IP);
    void CreateRoomToServer_Implementation(const FString& Name, const FString& IP);
public:
    void CreateRoomRequest(const FString& Name, const FString& IP);

protected:
    UFUNCTION(NetMulticast, Reliable)
    void UpdateRoomListToClient(const FString& Name, const FString& IP);
    void UpdateRoomListToClient_Implementation(const FString& Name, const FString& IP);
public:
    void UpdateRoomListResponse(const FString& Name, const FString& IP);

protected:
    UFUNCTION(Client, Unreliable)
    void InitializeRoomListToClient();
    void InitializeRoomListToClient_Implementation();
public:
    void InitializeRoomListResponse();

private:
    // TODO: server 전용, 리펙터링 필요
    TArray<FString> RoomNameList;
    TArray<FString> RoomIpList;

public:
    ULoginUI*              LoginUI;
    ULobbyUI*              LobbyUI;
    UCharacterCustomizeUI* CharacterCustomizeUI;
    UMessageBoxUI*         MessageBoxUI;
    UInputBoxUI*           CreateRoomUI;

private:
    FPlayerPreset Selected    = { 0 }; //!< 현재 고른 캐릭터 정보
    int32         SelectIndex = 0;     //!< 캐릭터 리스트 뷰에서 선택한 번호
    int32         SelectMax   = 0;     //!< 캐릭터 선택 가능 수

private:
    FString SelectedRoomAddress;
    FString SelectedRoomName;

private:
    int LookKinds[EPB_Count][EPL_Count] = { 0 }; //!< 각 Body의 Outfit별 아이템 가짓수

private:
    ALobbyActor*  Actor;
    ACameraActor* Viewer;

private:
    TSubclassOf<UUserWidget> LoginUiClass;
    TSubclassOf<UUserWidget> LobbyUiClass;
    TSubclassOf<UUserWidget> CharacterCustomizeUiClass;
    TSubclassOf<UUserWidget> MessageBoxUiClass;
    TSubclassOf<UUserWidget> InputBoxUiClass;
};