#pragma once

// use C++ standard for MySQL
#include <string>
#include <memory>

// MySQL
#include <ThirdParty/MySqlConnector/include/mysql/jdbc.h>

#include "CoreMinimal.h"
#include "Manager/Manager.h"
#include "DatabaseManager.generated.h"

/*********************************************************************************************************************
 * @brief 서버 기본 컨트롤러입니다.
 *********************************************************************************************************************/
UCLASS()
class META_API UDatabaseManager : public UManager {
	GENERATED_BODY()

public:
	using FConnection     = TSharedPtr<sql::Connection>;
	using FConnectionPool = TQueue<FConnection>;

public:
	void Initialize(FSubsystemCollectionBase&) override;

public:
	static constexpr int POOL_SIZE = 8; //!< connection pool size

public:
	void AddAccount(const FString& ID, FString& PW);

public:
	/**
	 * @brief call query
	 * 
	 * @param [in] Query   e.g. "SELECT * FROM TABLE WHERE KEY = ?"
	 * @param [in] Prepare e.g  [](sql::PreparedStatement* In) { In->setInt(1, number); }
	 * @param [in] Process e.g. [](sql::ResultSet* In) { ... }
	 */
	bool Query(
		const FString& Query,
		TFunction<void(sql::PreparedStatement*)> Prepare,
		TFunction<void(sql::ResultSet*)> Process
	);

public:
	TFuture<FConnection> GetConnection();
	TFuture<void> 		 ReleaseConnection(FConnection);

public:
	// Default Parameter temporary
	void Setup(
		const FString& DBUser     = _T("root"),
		const FString& DBPassword = _T("0000"),
		const FString& DBName     = _T("DB"),
		const FString& DBAddress  = _T("tcp://127.0.0.1:3306")
	);

private:
	FCriticalSection CS;
	FConnectionPool  Pool;

private:
	FString Host;
	FString User;
	FString PW;
	FString DB;
};