// Fill out your copyright notice in the Description page of Project Settings.

#include "Generic/GenericCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Manager/PlayerMeshManager.h"

// Sets default values
AGenericCharacter::AGenericCharacter() {
    PrimaryActorTick.bCanEverTick = true;

    // MeshComponent
    BodyMesh = GetMesh();
    check(BodyMesh);
    BodyMesh->SetWorldLocation(FVector{ 0, 0, -90 });
    BodyMesh->SetWorldRotation(FRotator{ 0, -90, 0, });
    for (int i = 0; i < EPL_Count; ++i) {
        LookMesh[i] = CreateDefaultSubobject<USkeletalMeshComponent>(*FString::Printf(_T("LookMesh_%d"), i));
        check(LookMesh[i]);
    }

    // Set hierarchy for same animation
    BodyMesh->SetupAttachment(RootComponent);
    for (int i = 0; i < EPL_Count; ++i) {
        LookMesh[i]->SetupAttachment(BodyMesh);
        LookMesh[i]->SetLeaderPoseComponent(BodyMesh, true);
    }

    // Set Camera
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(_T("SpringArm"));
    check(SpringArm);
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(_T("Camera"));
    check(Camera);
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;

    SetTopView();
}

AGenericCharacter::~AGenericCharacter() {}

//auto AGenericCharacter::GetState() const->EPlayerState {
//    if (IsJump) {
//
//    }
//    return EPS_Idle;
//}

// Called when the game starts or when spawned
void AGenericCharacter::BeginPlay() {
    Super::BeginPlay();
    
    PrimaryActorTick.bCanEverTick = true;
    bReplicates                   = true;

    BodyMesh->SetSkeletalMesh(UPlayerMeshManager::Instance(this)->GetBodyMesh(0));
    auto Anim = LoadObject<UAnimBlueprint>(
        nullptr,
        _T("/Script/Engine.AnimBlueprint'/Game/Animation/BP_MaleAnim.BP_MaleAnim'")
    );
    check(Anim);
    BodyMesh->SetAnimClass(Anim->GeneratedClass);

    IsRun = true; // 임시
}

// Called every frame
void AGenericCharacter::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AGenericCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// TODO: Axis 한 쪽 덮어써지는거 해결하기
void AGenericCharacter::Move(const FInputActionValue& Value) {
    check(Controller);

    Input = Value.Get<FVector2D>();
    if (Input.SizeSquared() > 0.0f) {
        FRotator Rotation = Controller->GetControlRotation();
        FRotator YawRotation(0, Rotation.Yaw, 0);

        FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        FVector Right   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        AddMovementInput(Forward, Input.X);
        AddMovementInput(Right,   Input.Y);
    }
}

void AGenericCharacter::Jump() {
    Super::Jump();
    IsJump = true;
}

void AGenericCharacter::Landed(const FHitResult& Hit) {
    Super::Landed(Hit);
    IsJump = false;
}

void AGenericCharacter::SetTopView() {
    SpringArm->TargetArmLength = 150;
    Camera->SetWorldRotation(FRotator{ -60, 0, 0 });
    SpringArm->SetWorldLocation(FVector{ 0, 0, 300 });
}

void AGenericCharacter::SetThirdPersonView() {
    SpringArm->TargetArmLength = 300;
    Camera->SetWorldRotation(FRotator{ 0, 0, 0 });
    SpringArm->SetWorldLocation(FVector{ 0, 0, 90 });
}
