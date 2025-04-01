// Fill out your copyright notice in the Description page of Project Settings.

#include "Generic/GenericCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Server/AccountManager.h"

// Sets default values
AGenericCharacter::AGenericCharacter() {
    UActorComponent* Ptr;

    Ptr = GetCharacterMovement();
    check(Ptr);

    Ptr = GetCapsuleComponent();
    check(Ptr);
}

AGenericCharacter::~AGenericCharacter() {}

// Called when the game starts or when spawned
void AGenericCharacter::BeginPlay() {
    Super::BeginPlay();
    
    PrimaryActorTick.bCanEverTick = true;
    bReplicates                   = true;

    USkeletalMesh* TempMesh =  LoadObject<USkeletalMesh>(nullptr, TEXT("/Script/Engine.SkeletalMesh'/Engine/EngineMeshes/SkeletalCube.SkeletalCube'"));
    GetMesh()->SetSkeletalMesh(TempMesh);
}

// Called every frame
void AGenericCharacter::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AGenericCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGenericCharacter::MoveRequst(const FVector& In) {
    //if (HasAuthority()) {
    //    SetActorLocation(In);
    //}
    //else {
    //    MoveResponse(In); // delegate to server
    //}
}

bool AGenericCharacter::MoveResponse_Validate(const FVector& In) {
    return true;
}

void AGenericCharacter::MoveResponse_Implementation(const FVector& In) {
    // SetActorLocation(In);
}
