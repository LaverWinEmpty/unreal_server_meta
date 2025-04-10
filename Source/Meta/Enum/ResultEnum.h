#pragma once

#include "CoreMinimal.h"

enum EResultCode : int8 {
    ERC_Uknown = -1,
    ERC_Succeeded, // 작업 성공

    ERC_CreatedAccount, // 계정 생성 성공
    ERC_UnknownID,      // 로그인 실패: 존재하지 않는 계정
    ERC_DuplicatedD,    // 계정 생성 실패: 이미 존재하는 계정
    ERC_InvalidID,      // 계정 생성 실패: 너무 짧거나 긴 ID
    ERC_InvalidPW,      // 계정 생성 실패: 너무 짧은 PW

    ERC_CreatedCharacter, // 캐릭터 생성 성송
    ERC_InvalidName,      // 캐릭터 생성 실패: 이름 오류
    ERC_DuplicatedName,   // 캐릭터 생성 실패: 플레이어 이름 중복
    ERC_UnknownName,      // 캐릭터 존재 하지 않음, 나중에 사용
};

inline const TCHAR* GetResultMessageFromCode(int8 In) {
    switch(In) {
        case ERC_Succeeded:        return _T("작업이 완료되었습니다.");
        case ERC_CreatedAccount:   return _T("계정이 생성되었습니다.");
        case ERC_UnknownID:        return _T("로그인 실패\n 존재하지 않는 계정입니다.");
        case ERC_DuplicatedD:      return _T("계정 생성 실패\n이미 존재하는 계정입니다.");
        case ERC_InvalidID:        return _T("계정 생성 실패\nID: 3 ~ 16자로 입력해주십시오.");
        case ERC_InvalidPW:        return _T("계정 생성 실패\nPW: 3자 이상 입력해주십시오.");
        case ERC_CreatedCharacter: return _T("캐릭터가 생성되었습니다.");
        case ERC_DuplicatedName:   return _T("캐릭터 생성 실패\n이미 존재하는 닉네임입니다.");
        case ERC_InvalidName:      return _T("캐릭터 생성 실패\nNickname: 1 ~8자로 입력해주십시오.");
        default: return _T("UNKNOWN ERROR");
    }
}