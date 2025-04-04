#pragma once

#include "CoreMinimal.h"

enum EAuthAction : int8 {
    EAA_LogIn,   // 접속
    EAA_LogOut,  // 종료
    EAA_SignUp,  // 가입
    EAA_SignOut, // 탈퇴
};

enum EAuthResult : int8 {
    EAR_Uknown = -1,
    EAR_Suceeded,     // 성공
    EAR_AlreadyExist, // 이미 존재하는 계정
    EAR_NotExist,     // 존재하지 않는 계정
};