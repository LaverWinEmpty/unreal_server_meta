#pragma once

#include "CoreMinimal.h"

enum EAuthAction : int8 {
    EAA_LogIn,   // 접속
    EAA_LogOut,  // 종료
    EAA_SignUp,  // 가입
    EAA_SignOut, // 탈퇴
};

inline FString ConvertActionTypeName(int8 In) {
    switch (In) {
        case EAA_LogIn:   return _T("Log-in");
        case EAA_LogOut:  return _T("Log-out");
        case EAA_SignUp:  return _T("Sign-in");
        case EAA_SignOut: return _T("Sign-out");
    };
    checkNoEntry();
    return "";
}