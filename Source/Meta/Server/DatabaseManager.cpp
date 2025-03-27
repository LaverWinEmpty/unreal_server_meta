#include "Server/DatabaseManager.h"
#include "Misc/ScopeLock.h"
#include "HAL/CriticalSection.h"



void UDatabaseManager::Initialize(FSubsystemCollectionBase& Collection) {

}

void UDatabaseManager::AddAccount(const FString& ID, FString& PW) {

}



std::shared_ptr<sql::Connection> MySQLConnectionPool::GetConnection() {
    FScopedLock LockGuard(&PoolCS);
    if (ConnectionPool.empty()) {
        return nullptr;
    }
    std::shared_ptr<sql::Connection> conn = ConnectionPool.front();
    ConnectionPool.pop();
    return conn;
}