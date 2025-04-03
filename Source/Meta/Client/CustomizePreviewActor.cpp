// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/CustomizePreviewActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Manager/PlayerMeshManager.h"
#include "Animation/AnimInstance.h"

ACustomizePreviewActor::ACustomizePreviewActor() {
	// 클라이언트에서 사용

	PrimaryActorTick.bCanEverTick = true;

    // Set Component

    Body    = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
    Emotion = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Emotion"));
    Upper   = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Upper"));
    Lower   = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Lower"));
    Shoes   = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shoes"));
    
    // Set hierarchy
    RootComponent = Body;
    Emotion->SetupAttachment(Body);
    Upper->SetupAttachment(Body);
    Lower->SetupAttachment(Body);
    Shoes->SetupAttachment(Body);

    // 동일한 애니메이션
    Emotion->SetLeaderPoseComponent(Body, true);
    Upper->SetLeaderPoseComponent(Body, true);
    Lower->SetLeaderPoseComponent(Body, true);
    Shoes->SetLeaderPoseComponent(Body, true);

    //static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(TEXT("/Game/Character/AnimBP_Mannequin.AnimBP_Mannequin_C"));
    //if (AnimBP.Succeeded())
    //{
    //    BodyMesh->SetAnimInstanceClass(AnimBP.Class);
    //    // Outfit은 Body의 애니메이션을 따라가므로 따로 설정할 필요 없음
    //}
}

void ACustomizePreviewActor::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ACustomizePreviewActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ACustomizePreviewActor::SetBodyMesh() {
    UPlayerMeshManager* Manager = UPlayerMeshManager::Instance(this);
    if (Body->GetSkeletalMeshAsset() == nullptr) {
        Body->SetSkeletalMesh(Manager->BodyAsset);
    }
}

void ACustomizePreviewActor::SetEmotionMesh(USkeletalMesh* In) {
    Emotion->SetSkeletalMesh(In);
}

void ACustomizePreviewActor::SetUpperMesh(USkeletalMesh* In) {
    Upper->SetSkeletalMesh(In);
}

void ACustomizePreviewActor::SetLowerMesh(USkeletalMesh* In) {
    Lower->SetSkeletalMesh(In);
}

void ACustomizePreviewActor::SetShoesMesh(USkeletalMesh* In) {
    Shoes->SetSkeletalMesh(In);
}

