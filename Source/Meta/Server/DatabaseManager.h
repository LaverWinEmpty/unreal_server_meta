#pragma once

#pragma push_macro("check")       // backup Unreal check
#undef check                      // Unreal macro conflicts with MySQL function
#ifndef CONCPP_BUILD_SHARED       // MySQL build option
#    define CONCPP_BUILD_SHARED 1 // not build, dummy value
#    include <mysql/jdbc.h>       // include MySQL
#    undef CONCPP_BUILD_SHARED    // undef for safe
#else
#    include <mysql/jdbc.h>
#endif
#pragma pop_macro("check")

#include "CoreMinimal.h"
#include "Manager/Manager.h"
#include "HAL/CriticalSection.h"
#include "Async/Async.h"
#include "DatabaseManager.generated.h"

/*********************************************************************************************************************
 * @brief DB 매니저입니다.
 *********************************************************************************************************************/
//namespace sql {
//	class PreparedStatement;
//	class ResultSet;
//	class Connection;
//}

UCLASS()
class META_API UDatabaseManager : public UManager {
	GENERATED_BODY()

public:
	DECLARE_MANAGER_GET_INSTANCE(UDatabaseManager);

public:
	static constexpr int POOL_SIZE = 8; //!< connection pool size

public:
	using FConnection     = TSharedPtr<sql::Connection>;
	using FConnectionPool = TQueue<FConnection>;

public:
	void Initialize(FSubsystemCollectionBase&) override;

public:
	/**
	 * @brief call query
	 * @note  async, lambda recommanded copy
	 * @param [in] Query   e.g. "SELECT * FROM TABLE WHERE KEY = ?"
	 * @param [in] Prepare e.g  [=](sql::PreparedStatement* In) { In->setInt(1, number); }
	 * @param [in] Process e.g. [&](sql::ResultSet* In) { while(In->next()) { ... } }
	 */
	void Query(
		const FString&                           Query,
		TFunction<void(sql::PreparedStatement*)> Prepare = nullptr,
		TFunction<void(sql::ResultSet*)>         Process = nullptr
	);

public:
	TFuture<FConnection> GetConnection();
	TFuture<void> 		 ReleaseConnection(FConnection);

private:
	void OnQuery(
		sql::Connection& Connection,
		const sql::SQLString& Query,
		TFunction<void(sql::PreparedStatement*)> Prepare,
		TFunction<void(sql::ResultSet*)> Process
	);


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