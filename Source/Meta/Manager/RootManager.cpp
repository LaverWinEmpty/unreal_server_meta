#include "Manager/RootManager.h"
#include "Server/DatabaseManager.h"

// ...

void URootManager::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);

	UManager::Instance<UDatabaseManager>(this)->Setup();
}
