//#include "Server/AccountManager.h"
//#include "Server/DatabaseManager.h"
//#include "Client/LoginController.h"
//#include "Server/ClientSessionManager.h"
//#include "Generic/GenericGame.h"
//#include "Misc/SecureHash.h"
//// #include "Actor/GenericCharacter.h"
//// #include "EncryptionContextOpenSSL.h"
//
//
//
////AGenericGame* GameMode = Cast<AGenericGame>(GetWorld()->GetAuthGameMode());
////if (!GameMode) {
////    UE_LOG(LogTemp, Error, TEXT("게임모드를 로딩하는데 실패했습니다."));
////}
//
//// TODO: 임시 로직입니다. 추후 Client Session 등으로 빼야 합니다.
//// TODO: 컨트롤러 생성 후 캐릭터 만들어서 할당
//// GetWorldSafe(this)->SpawnActor<AGenericController>(AGenericController::StaticClass());
//
////// TODO: 임시 로직입니다. 추후 Client Session 등으로 빼야 합니다.
////FActorSpawnParameters SpawnParams{};
////AActor* Player = GetWorld()->SpawnActor<AGenericCharacter>(FVector{ 100, 100, 100 }, FRotator{ 0, 0, 0 }, SpawnParams);
////if (!Player) {
////    UE_LOG(LogTemp, Error, TEXT("캐릭터 스폰 실패"));
////}
////else UE_LOG(LogTemp, Log, TEXT("캐릭터 스폰"));