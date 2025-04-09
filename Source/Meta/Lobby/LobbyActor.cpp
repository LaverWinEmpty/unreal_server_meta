// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Manager/PlayerMeshManager.h"
#include "Animation/AnimInstance.h"

ALobbyActor::ALobbyActor() {
	// 클라이언트에서 사용

	PrimaryActorTick.bCanEverTick = true;

    // Set Component
    Body = CreateDefaultSubobject<USkeletalMeshComponent>(_T("Body"));
    for (int i = 0; i < EPL_Count; ++i) {
        Outfit[i] = CreateDefaultSubobject<USkeletalMeshComponent>(*FString::Printf(_T("Outfit_%d"), i));
    }
    
    // Set hierarchy for same animation
    RootComponent = Body;
    for (int i = 0; i < EPL_Count; ++i) {
        Outfit[i]->SetupAttachment(Body);
        Outfit[i]->SetLeaderPoseComponent(Body, true);
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
    Body->SetSkeletalMesh(In);
}

void ALobbyActor::SetLookMesh(int Index, USkeletalMesh* In) {
    Outfit[Index]->SetSkeletalMesh(In);
}

void ALobbyActor::PlayAnimation(UAnimationAsset* In) {
    Body->PlayAnimation(In, true);
}