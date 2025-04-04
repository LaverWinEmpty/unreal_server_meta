#include "Server/DatabaseManager.h"
#include "Async/Async.h"
#include "Misc/ScopeLock.h"

void UDatabaseManager::Initialize(FSubsystemCollectionBase& Collection) {
    Super::Initialize(Collection);
    Setup();
}

void UDatabaseManager::Setup(
    const FString& DBUser,
    const FString& DBPassword,
    const FString& DBName,
    const FString& DBAddress
) {
    if (!IsServer(this)) {
        return;
    }

    User = DBUser;
    PW   = DBPassword;
    DB   = DBName;
    Host = DBAddress;

    sql::mysql::MySQL_Driver* Driver = sql::mysql::get_mysql_driver_instance();
    check(Driver);

    for (int32 i = 0; i < POOL_SIZE; ++i) {
        TSharedPtr<sql::Connection> Connection(
            Driver->connect(TCHAR_TO_UTF8(*Host), TCHAR_TO_UTF8(*DBUser), TCHAR_TO_UTF8(*DBPassword))
        );
        Connection->setSchema(TCHAR_TO_UTF8(*DB)); // connection
        Pool.Enqueue(Connection);                  // enqueue
    }

    //// TODO: 언리얼에서 처리할 수 있도록 수정해야 함
    //catch (sql::SQLException& e) {
    //    UE_LOG(LogTemp, Error, TEXT("MySQL Connection Pool Initialization Failed: %s"), UTF8_TO_TCHAR(e.what()));
    //    return;
    //}

    auto CheckTableExists = [](sql::ResultSet* In, const FString& TableName) {
        if (In->next()) {
            ensureMsgf(In->getInt(1) > 0, TEXT("Table '%s' does not exist!"), *TableName);
        }
    };

    // 테이블 검사
    Query("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = 'db' AND table_name = 'user_tbl'",
        nullptr,
        [CheckTableExists](sql::ResultSet* In) {
            CheckTableExists(In, "user_tbl");
        }
    );

    // 테이블 검사
    Query("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = 'db' AND table_name = 'player_tbl'",
        nullptr,
        [CheckTableExists](sql::ResultSet* In) {
            CheckTableExists(In, "player_tbl");
        }
    );
}

// 풀버전
void UDatabaseManager::Query(
    const FString&                           In,
    TFunction<void(sql::PreparedStatement*)> Prepare,
    TFunction<void(sql::ResultSet*)>         Process
) {
    check(IsServer()); // DB는 Server에서 실행되어야 함

    sql::SQLString UTF8Query = TCHAR_TO_UTF8(*In); // convert
    // async
    TFuture<FConnection> Connection = GetConnection();
    Connection.Then(
        [this, UTF8Query, Prepare, Process](TFuture<FConnection> In) {
            FConnection Connection = In.Get();
            OnQuery(*Connection, UTF8Query, Prepare, Process);
            ReleaseConnection(Connection);
        } // end lambda
    ); // end call Connection.Then
} // end declare Execute

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

TFuture<void> UDatabaseManager::ReleaseConnection(FConnection In) {
    return Async(EAsyncExecution::TaskGraph,
        [this, In]() {
            FScopeLock LockGuard(&CS);
            if (In.IsValid()) {
                Pool.Enqueue(In);
            }
        }
    );
}

void UDatabaseManager::OnQuery(
    sql::Connection& Connection,
    const sql::SQLString& Query,
    TFunction<void(sql::PreparedStatement*)> Prepare,
    TFunction<void(sql::ResultSet*)> Process
) {
    UE_LOG(LogTemp, Log, _T("Acquired a MySQL Connection"));

    sql::PreparedStatement* Statement = nullptr;
    sql::ResultSet*         ResultSet = nullptr;

    // set query parameter
    Statement = Connection.prepareStatement(Query);
    if (!Statement) {
        UE_LOG(LogTemp, Error, TEXT("Query PreparedStatement Create Failed"));
        check(false);
        return;
    }

    // else ignore
    if (Prepare) {
        Prepare(Statement); // throw: C++ std except
    }

    // get result and process
    ResultSet = Statement->executeQuery();
    if (!ResultSet) {
        UE_LOG(LogTemp, Error, TEXT("Query ResultSet Create Failed"));
        check(false);
        return;
    }

    // else ignore
    if (Process) {
        Process(ResultSet); // throw: C++ std except
    }

    // release
    delete ResultSet;
    delete Statement;

    UE_LOG(LogTemp, Log, TEXT("Query executed successfully."));
}
