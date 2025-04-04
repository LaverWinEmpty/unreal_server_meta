#pragma once

#include "CoreMinimal.h"

enum EResultCode : int8 {
    ERC_Uknown = -1,
    ERC_Succeeded,        // 성공
    ERC_AlreadyExistID,   // 이미 존재하는 계정
    ERC_NotExistID,       // 존재하지 않는 계정
    ERC_AlreadyExistName, // 플레이어 이름 중복
};

inline const TCHAR* GetResultMessage(int8 In) {
    switch(In) {
        case ERC_Succeeded:        return _T("작업이 완료되었습니다.");
        case ERC_AlreadyExistID:   return _T("이미 존재하는 계정입니다.");
        case ERC_NotExistID:       return _T("존재하지 않는 계정입니다.");
        case ERC_AlreadyExistName: return _T("이미 존재하는 이름입니다.");
        default: return _T("UNKNOWN ERROR");
    }
}