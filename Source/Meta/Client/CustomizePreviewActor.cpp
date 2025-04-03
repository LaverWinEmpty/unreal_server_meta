// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/CustomizePreviewActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Manager/PlayerMeshManager.h"
#include "Animation/AnimInstance.h"

ACustomizePreviewActor::ACustomizePreviewActor() {
	// 클라이언트에서 사용

	PrimaryActorTick.bCanEverTick = true;

    // Set Component
    Body = CreateDefaultSubobject<USkeletalMeshComponent>(_T("Body"));
    for (int i = 0; i < EPO_OutfitCount; ++i) {
        Outfit[i] = CreateDefaultSubobject<USkeletalMeshComponent>(*FString::Printf(_T("Outfit_%d"), i));
    }
    
    // Set hierarchy for same animation
    RootComponent = Body;
    for (int i = 0; i < EPO_OutfitCount; ++i) {
        Outfit[i]->SetupAttachment(Body);
        Outfit[i]->SetLeaderPoseComponent(Body, true);
    }
}

void ACustomizePreviewActor::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ACustomizePreviewActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ACustomizePreviewActor::SetBodyMesh(USkeletalMesh* In) {
    Body->SetSkeletalMesh(In);
}

void ACustomizePreviewActor::SetOutfitMesh(int Index, USkeletalMesh* In) {
    Outfit[Index]->SetSkeletalMesh(In);
}

void ACustomizePreviewActor::PlayAnimation(UAnimationAsset* In) {
    Body->PlayAnimation(In, true);
}