// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enum/PlayerMeshEnum.h"
#include "LobbyActor.generated.h"

class USkeletalMeshComponent;

UCLASS()
class META_API ALobbyActor : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void SetBodyMesh(USkeletalMesh*);
	void SetLookMesh(int EPO, USkeletalMesh*);
	void PlayAnimation(UAnimationAsset*);

public:
	USkeletalMeshComponent*   BodyMesh;
	USkeletalMeshComponent*   LookMesh[EPL_Count];
};
