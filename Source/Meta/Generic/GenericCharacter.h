// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum/PlayerMeshEnum.h"
#include "GameFramework/Character.h"
#include "GenericCharacter.generated.h"

UCLASS()
class META_API AGenericCharacter: public ACharacter {
    GENERATED_BODY()

public:
    AGenericCharacter();
    ~AGenericCharacter();

public:
    USkeletalMeshComponent* Body;
    USkeletalMeshComponent* Outfit[EPL_Count];

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
    UFUNCTION(Server, Reliable, WithValidation)
    void MoveResponse(const FVector& In); //!< move request accept from server
    void MoveRequst(const FVector& In);   //!< move request from client

protected:
    bool MoveResponse_Validate(const FVector& In);
    void MoveResponse_Implementation(const FVector& In);
};
