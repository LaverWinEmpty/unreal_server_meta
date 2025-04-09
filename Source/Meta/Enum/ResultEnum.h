#pragma once

#include "CoreMinimal.h"

enum EResultCode : int8 {
    ERC_Uknown = -1,
    ERC_Succeeded,        // 성공 // 잠깐 지움

    ERC_CreatedID,   // ID 생성 성공
    ERC_InvalidID,   // 존재하지 않는 계정
    ERC_DuplicatedD, // 이미 존재하는 계정

    ERC_CreatedCharacter, // 캐릭터 생성 정상적
    ERC_DuplicatedName,   // 플레이어 이름 중복
    ERC_InvalidName,      // 이름 오류
};

inline const TCHAR* GetResultMessage(int8 In) {
    switch(In) {
        // case ERC_Succeeded:        return _T("작업이 완료되었습니다.");
        case ERC_CreatedID:        return _T("계정이 생성되었습니다.");
        case ERC_InvalidID:        return _T("존재하지 않는 계정입니다.");
        case ERC_DuplicatedD:      return _T("이미 존재하는 계정입니다.");
        case ERC_CreatedCharacter: return _T("캐릭터가 생성되었습니다.");
        case ERC_DuplicatedName:   return _T("이미 존재하는 이름입니다.");
        case ERC_InvalidName:      return _T("이름이 너무 깁니다.");
        default: return _T("UNKNOWN ERROR");
    }
}