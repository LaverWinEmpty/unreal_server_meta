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
class META_API UDatabaseManager : public UManager
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase&) override;

public:
	static constexpr int POOL_SIZE = 8; //!< connection pool size

public:
	void AddAccount(const FString& ID, FString& PW);

private:
	FCriticalSection PoolCS;

public:
	const std::string Host = "tcp://127.0.0.1:3306";
	const std::string User = "root";
	const std::string ID   = "DB";
	const std::string PW   = "0000";
};
