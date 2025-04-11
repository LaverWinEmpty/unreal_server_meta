#pragma once

#include "CoreMinimal.h"
#include "Manager/Manager.h"
#include "Data/ClientSession.h"
#include "ClientSessionManager.generated.h"

UCLASS()
class META_API UClientSessionManager : public UManager
{
	GENERATED_BODY()

public:
	DECLARE_MANAGER_GET_INSTANCE(UClientSessionManager);

public:
	bool IsExist(const FString&);

public:
	void Enter(const APlayerController*, TSharedPtr<FClientSession>);
	void Leave(const APlayerController*);
	void Leave(const FString& ID);

public:
	template<typename... Args> static TSharedPtr<FClientSession> Create(Args&&...);

public:
	TSharedPtr<FClientSession>     GetClientSession(APlayerController*);
	TWeakObjectPtr<UNetConnection> GetSokcet(const FString&);
	
private:
	TMap<TWeakObjectPtr<UNetConnection>, TSharedPtr<FClientSession>> ClientSessionList;
	TMap<FString, TWeakObjectPtr<UNetConnection>>                    ClientSocketList;
};

template<typename ...Args>
inline TSharedPtr<FClientSession> UClientSessionManager::Create(Args&&... Param) {
	return MakeShared<FClientSession>(Forward<Args>(Param)...);
}
