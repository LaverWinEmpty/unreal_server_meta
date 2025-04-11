// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Async/Async.h"
#include "Manager.generated.h"

/*********************************************************************************************************************
 * Manager Base Class with Utilities
 *********************************************************************************************************************/

#define TODO(text) checkf(false, _T("TODO: %s"), _T(##text));

#define DECLARE_MANAGER_GET_INSTANCE(TYPE)\
    static TYPE* Instance(const UObject* Context) { return UManager::Instance<TYPE>(Context); }

UCLASS()
class META_API UManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	template<typename T> static T* Instance(const UObject* In); //!< get subsystem

public:
	//!@ brief safe get world
	static UWorld* GetWorldSafe(const UObject*);
	
public:
	ENetMode        GetNetMode() const;         //!< GetWorld()->GetNetMode();
	static ENetMode GetNetMode(const UObject*); //!< GetWorld()->GetNetMode();

public:
	const TCHAR*        GetNetModeString() const;         //!< GetNodeMode() to string
	static const TCHAR* GetNetModeString(const UObject*); //!< GetNodeMode() to string
	static const TCHAR* GetNetModeString(ENetMode);       //!< ENetMode argument to string

public:
	bool IsUser()   const; //!< check not dedicated
	bool IsServer() const; //!< check not client
	bool IsHost()   const; //!< check listen and standalone

public:
	bool IsClient()     const; //!< check client
	bool IsStandalone() const; //!< check standalone
	bool IsListen()     const; //!< check listen
	bool IsDedicated()  const; //!< check dedicated

public:
	static bool IsUser(const UObject*);   //!< check not dedicated
	static bool IsServer(const UObject*); //!< check not client
	static bool IsHost(const UObject*);   //!< check listen and standalone

public:
	static bool IsClient(const UObject*);     //!< check client
	static bool IsStandalone(const UObject*); //!< check standalone
	static bool IsListen(const UObject*);     //!< check listen
	static bool IsDedicated(const UObject*);  //!< check dedicated

public:
	static const FString& GetServerAddress();   //!< e.g. return "127.0.0.1:7777"
	static const FString& GetDatabaseAddress(); //!< e.g. return "127.0.0.1:7777"

private:
	static FString GetAddress(const FString&, const FString&);

public:
	template<ENamedThreads::Type = ENamedThreads::GameThread>
	static void Dispatch(TFunction<void()>);

public:
	template<ENamedThreads::Type = ENamedThreads::GameThread, typename Callable, typename... Args>
	static void Dispatch(Callable&&, Args&&...);
};

template<typename T> inline T* UManager::Instance(const UObject* WorldContext) {
	return WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<T>();
}

template<ENamedThreads::Type TARGET>
inline void UManager::Dispatch(TFunction<void()> Lambda) {
	if (FTaskGraphInterface::Get().GetCurrentThreadIfKnown() == TARGET) {
		Lambda();
	}
	else AsyncTask(TARGET,
		[Lambda = MoveTemp(Lambda)]() mutable {
			Lambda();
		}
	);
}

template<ENamedThreads::Type TARGET, typename Callable, typename ...Args>
inline void UManager::Dispatch(Callable&& Procedure, Args&&... Arguments) {
	if (FTaskGraphInterface::Get().GetCurrentThreadIfKnown() == TARGET) {
		Procedure(Forward<Args>(Arguments)...);
	}
	else AsyncTask(TARGET,
		[Func = MoveTemp(Procedure), ...Param = Forward<Args>(Arguments)]() mutable {
			Func(Forward<Args>(Param)...);
		}
	);
}