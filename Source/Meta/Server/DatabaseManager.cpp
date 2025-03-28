#include "Server/DatabaseManager.h"
#include "Misc/ScopeLock.h"

#include "mysqlx/common.h"

void UDatabaseManager::Initialize(FSubsystemCollectionBase& Collection) {

}

void UDatabaseManager::AddAccount(const FString& UserID, FString& UserPW) {

}

void UDatabaseManager::Query(
    const FString&                           Query,
    TFunction<void(sql::PreparedStatement*)> Prepare,
    TFunction<void(sql::ResultSet*)>         Process) {
    // TODO: thread-pool 넣고 처리

    //TFuture<FConnection> Future = GetConnection();
    //Future.Next(
    //    // copy: async
    //    [this, Query, Prepare, Process](FConnection Connection) {
    //        if (Connection) {
    //            UE_LOG(LogTemp, Log, TEXT("Acquired a MySQL connection!")); // logging

    //            // set query parameter
    //            sql::PreparedStatement* Statement = Connection->prepareStatement(TCHAR_TO_UTF8(*Query));
    //            if (!Statement) {
    //                UE_LOG(LogTemp, Error, TEXT("Query PreparedStatement Create Failed"));
    //                check(false);
    //                return;
    //            }
    //            Prepare(Statement);

    //            // get result and process
    //            sql::ResultSet* ResultSet = Statement->executeQuery();
    //            if (!ResultSet) {
    //                UE_LOG(LogTemp, Error, TEXT("Query ResultSet Create Failed"));
    //                check(false);
    //                return;
    //            }
    //            Process(ResultSet);

    //            // terminate
    //            UE_LOG(LogTemp, Log, TEXT("Query executed successfully."));

    //            // release
    //            delete ResultSet;
    //            delete Statement;
    //            ReleaseConnection(
    //                Connection).Next([]() {
    //                    UE_LOG(LogTemp, Log, TEXT("Released the MySQL connection!"));
    //                }
    //            );
    //        }

    //        // failed
    //        else {
    //            UE_LOG(LogTemp, Error, TEXT("Failed to acquire a MySQL connection"));
    //        }
    //    }
    //);
}

void UDatabaseManager::Setup(const FString& DBUser, const FString& DBPassword, const FString& DBName, const FString& DBAddress) {
    User = DBUser;
    PW   = DBPassword;
    DB   = DBName;
    Host = DBAddress;

    //sql::mysql::MySQL_Driver* Driver = sql::mysql::get_mysql_driver_instance();
    //check(Driver);

    //for (int32 i = 0; i < POOL_SIZE; ++i) {
    //    TSharedPtr<sql::Connection> Connection(
    //        Driver->connect(TCHAR_TO_UTF8(*Host), TCHAR_TO_UTF8(*DBUser), TCHAR_TO_UTF8(*DBPassword))
    //    );
    //    Connection->setSchema(TCHAR_TO_UTF8(*DB)); // connection
    //    Pool.Enqueue(Connection);                  // enqueue
    //}

    //// TODO: 언리얼에서 처리할 수 있도록 수정해야 함
    //catch (sql::SQLException& e) {
    //    UE_LOG(LogTemp, Error, TEXT("MySQL Connection Pool Initialization Failed: %s"), UTF8_TO_TCHAR(e.what()));
    //    return;
    //}
}

auto UDatabaseManager::GetConnection()->TFuture<FConnection> {
    return Async(EAsyncExecution::TaskGraph,
        [this]()->FConnection {
            FScopeLock LockGuard(&CS);
            if (Pool.IsEmpty()) {
                return nullptr;
            }
            FConnection Connection;
            Pool.Dequeue(Connection);
            return Connection;
        }
    );
}

TFuture<void>  UDatabaseManager::ReleaseConnection(FConnection In) {
    return Async(EAsyncExecution::TaskGraph,
        [this, In]() {
            FScopeLock LockGuard(&CS);
            if (In.IsValid()) {
                Pool.Enqueue(In);
            }
        }
    );
}

