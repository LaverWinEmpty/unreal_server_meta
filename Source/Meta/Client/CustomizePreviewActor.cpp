// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/CustomizePreviewActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

ACustomizePreviewActor::ACustomizePreviewActor() {
	// 클라이언트에서 사용

	PrimaryActorTick.bCanEverTick = true;

    // Set Component
    Body    = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
    Emotion = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Emotion"));
    Outfit  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Outfit"));
    
    // Set hierarchy
    RootComponent = Body;
    Emotion->SetupAttachment(Body);
    Outfit->SetupAttachment(Body);

    // 동일한 애니메이션
    Outfit->SetLeaderPoseComponent(Body, true);
    Emotion->SetLeaderPoseComponent(Body, true);

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

