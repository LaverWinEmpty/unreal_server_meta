// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum/PlayerMeshEnum.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GenericCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class META_API AGenericCharacter: public ACharacter {
    GENERATED_BODY()

public:
    enum EPlayerState {
        EPS_Idle,
        EPS_Walk,
        EPS_Run,

        EPS_JumpBegin,
        EPS_JumpLoop,
        EPS_JumpEnd,
    };

public:
    AGenericCharacter();
    ~AGenericCharacter();

public:
    //UFUNCTION(BlueprintCallable)
    //EPlayerState GetState() const;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
    void Move(const FInputActionValue&);
    void Jump() override;
    void Landed(const FHitResult&) override;

public:
    void SetTopView();
    void SetThirdPersonView();

public:
    USpringArmComponent*    SpringArm;
    UCameraComponent*       Camera;
    USkeletalMeshComponent* BodyMesh;
    USkeletalMeshComponent* LookMesh[EPL_Count];

public:
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Input") FVector2D Input;

public:
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Input") bool IsRun  = false;
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Input") bool IsJump  = false;
};
