#include "Manager/Manager.h"

UWorld* UManager::GetWorldSafe(const UObject* Context) {
    check(Context);
    UWorld* World = Context->GetWorld();
#ifdef NDEBUG
    check(World);
#else
    ensure(World);
#endif
    return World;
}

ENetMode UManager::GetNetMode() const {
    return GetWorldSafe(this)->GetNetMode();
}

ENetMode UManager::GetNetMode(const UObject* In) {
    return GetWorldSafe(In)->GetNetMode();
}

const TCHAR* UManager::GetNetModeString() const {
    return GetNetModeString(GetNetMode());
}

const TCHAR* UManager::GetNetModeString(const UObject* In) {
    return GetNetModeString(GetNetMode(In));
}

const TCHAR* UManager::GetNetModeString(ENetMode In) {
    switch (In) {
    case ENetMode::NM_Standalone:      return _T("Standalone");
    case ENetMode::NM_Client:          return _T("Client");
    case ENetMode::NM_ListenServer:    return _T("Listen");
    case ENetMode::NM_DedicatedServer: return _T("Deadicated");
    case ENetMode::NM_MAX: checkNoEntry();
    }
    checkNoEntry();
    return _T("");
}


bool UManager::IsUser() const {
    return GetNetMode() != ENetMode::NM_DedicatedServer;
}

bool UManager::IsServer() const {
    return GetNetMode() != ENetMode::NM_Client;
}

bool UManager::IsHost() const {
    ENetMode NetMode = GetNetMode();
    return NetMode == NM_Standalone || NetMode == NM_ListenServer;
}

bool UManager::IsClient() const {
    return GetNetMode() == ENetMode::NM_Client;
}

bool UManager::IsStandalone() const {
    return GetNetMode() == ENetMode::NM_Standalone;
}

bool UManager::IsListen() const {
    return GetNetMode() == ENetMode::NM_ListenServer;
}

bool UManager::IsDedicated() const {
    return GetNetMode() == ENetMode::NM_DedicatedServer;
}

bool UManager::IsUser(const UObject* In) {
    return GetNetMode(In) != ENetMode::NM_DedicatedServer;
}

bool UManager::IsServer(const UObject* In) {
    return GetNetMode(In) != ENetMode::NM_Client;
}

bool UManager::IsHost(const UObject* In) {
    ENetMode NetMode = GetNetMode(In);
    return NetMode == NM_Standalone || NetMode == NM_ListenServer;
}

bool UManager::IsClient(const UObject* In) {
    return GetNetMode(In) == ENetMode::NM_Client;
}

bool UManager::IsStandalone(const UObject* In) {
    return GetNetMode(In) == ENetMode::NM_Standalone;
}

bool UManager::IsListen(const UObject* In) {
    return GetNetMode(In) == ENetMode::NM_ListenServer;
}

bool UManager::IsDedicated(const UObject* In) {
    return GetNetMode(In) == ENetMode::NM_DedicatedServer;
}

const FString& UManager::GetServerAddress() {
    static const FString Cache = GetAddress(_T("DedicatedIP"), _T("DedicatedPort"));
    return Cache;
}

const FString& UManager::GetDatabaseAddress() {
    static const FString Cache = GetAddress(_T("DatabaseIP"), _T("DatabasePort"));
    return Cache;
}

FString UManager::GetAddress(const FString& IpINI, const FString& PortINI) {
    check(GConfig);

    static const FString Section = _T("NetConfig");

    FString IP, Port;
    IP.Reserve(22);  // length of "255.255.255.255:65535\0" 
    Port.Reserve(6); // length of "65535\0"

    GConfig->GetString(*Section, *IpINI,   IP,   GGameIni);
    GConfig->GetString(*Section, *PortINI, Port, GGameIni);

    return IP + ":" + Port;
}
