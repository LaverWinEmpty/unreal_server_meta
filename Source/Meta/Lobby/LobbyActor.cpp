// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/Image.h"
#include "Animation/AnimInstance.h"
#include "Manager/PlayerMeshManager.h"

ALobbyActor::ALobbyActor() {
	// 클라이언트에서 사용

	PrimaryActorTick.bCanEverTick = true;

    // Set Component
    BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(_T("BodyMesh"));
    check(BodyMesh);
    for (int i = 0; i < EPL_Count; ++i) {
        LookMesh[i] = CreateDefaultSubobject<USkeletalMeshComponent>(*FString::Printf(_T("LookMesh_%d"), i));
        check(LookMesh[i]);
    }
    
    // Set hierarchy for same animation
    RootComponent = BodyMesh;
    for (int i = 0; i < EPL_Count; ++i) {
        LookMesh[i]->SetupAttachment(BodyMesh);
        LookMesh[i]->SetLeaderPoseComponent(BodyMesh, true);
    }
}

void ALobbyActor::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ALobbyActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ALobbyActor::SetBodyMesh(USkeletalMesh* In) {
    BodyMesh->SetSkeletalMesh(In);
}

void ALobbyActor::SetLookMesh(int Index, USkeletalMesh* In) {
    LookMesh[Index]->SetSkeletalMesh(In);
}

void ALobbyActor::PlayAnimation(UAnimationAsset* In) {
    BodyMesh->PlayAnimation(In, true);
}