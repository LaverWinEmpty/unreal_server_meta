#pragma once

#include "CoreMinimal.h"
#include "Manager/Manager.h"
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
	TWeakObjectPtr<UNetConnection> GetPlayerSocket(FString& ID);
	FStringView                    GetPlayerID(APlayerController*);

public:
	void OnLogIn(const APlayerController*, const FString&);
	void OnLogOut(const APlayerController*);
	void OnLogOut(const FString&);

private:
	TMap<FString, TWeakObjectPtr<UNetConnection>> ClientSession;
	TMap<TWeakObjectPtr<UNetConnection>, FStringView> ClientReference;
};
