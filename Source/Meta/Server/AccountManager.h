//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Manager/Manager.h"
//#include "AccountManager.generated.h"
//
///*********************************************************************************************************************
// * 로그인 관련
// *********************************************************************************************************************/
//
//UCLASS()
//class META_API UAccountManager : public UManager {
//    GENERATED_BODY()
//
//public:
//    DECLARE_MANAGER_GET_INSTANCE(UAccountManager);
//
//public:
//    enum EAuthAction : int8 {
//        EAA_LogIn,   // 접속
//        EAA_LogOut,  // 종료
//        EAA_SignUp,  // 가입
//        EAA_SignOut, // 탈퇴
//    };
//
//public:
//    enum EAuthResult : int8 {
//        EAR_Uknown = -1,
//        EAR_Suceeded,     // 성공
//        EAR_AlreadyExist, // 이미 존재하는 계정
//        EAR_NotExist,     // 존재하지 않는 계정
//    };
//
//private:
//    FString PasswordSHA256(const FString&);
//    static FString GetSalted(const FString&);
//};
//
//using EAuthAction = UAccountManager::EAuthAction;
//using EAuthResult = UAccountManager::EAuthResult;
//using enum EAuthAction;
//using enum EAuthResult;