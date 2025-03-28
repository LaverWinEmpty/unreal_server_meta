#pragma once

// //MySQL
// //TODO: shipping 관련 처리
// //TODO: CONCPP_BUILD_SHARED 처리
//#ifndef CONCPP_BUIDL_SHARED
//#	define CONCPP_BUILD_SHARED 0
//#endif
//#ifdef check
//#	undef check  // Unreal check
//#	include <ThirdParty/MySqlConnector/include/mysql/jdbc.h>
//#	ifdef UE_CHECK_IMPL
//#		define check(expr) UE_CHECK_IMPL(expr)  // Unreal check
//#	endif
//#else
//#	include <ThirdParty/MySqlConnector/include/mysql/jdbc.h>
//#endif
//#undef CONCPP_BUILD_SHARED

//#include "mysqlx/common.h"

// use C++ standard for MySQL
#include <string>
#include <memory>

#include "CoreMinimal.h"
#include "Manager/Manager.h"
#include "HAL/CriticalSection.h"
#include "Async//Async.h"
#include "DatabaseManager.generated.h"

/*********************************************************************************************************************
 * @brief DB 매니저입니다.
 *********************************************************************************************************************/
namespace sql {
	class PreparedStatement;
	class ResultSet;
	class Connection;
}

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
	void AddAccount(const FString& UserID, FString& UserPW);

public:
	/**
	 * @brief call query
	 * 
	 * @param [in] Query   e.g. "SELECT * FROM TABLE WHERE KEY = ?"
	 * @param [in] Prepare e.g  [](sql::PreparedStatement* In) { In->setInt(1, number); }
	 * @param [in] Process e.g. [](sql::ResultSet* In) { while(In->next()) { ... } }
	 */
	void Query(
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