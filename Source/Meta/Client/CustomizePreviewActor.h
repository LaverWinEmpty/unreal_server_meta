// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CustomizePreviewActor.generated.h"

class USkeletalMeshComponent;

UCLASS()
class META_API ACustomizePreviewActor : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACustomizePreviewActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void SetBodyMesh();
	void SetEmotionMesh(USkeletalMesh*);
	void SetUpperMesh(USkeletalMesh*);
	void SetLowerMesh(USkeletalMesh*);
	void SetShoesMesh(USkeletalMesh*);

public:
	USkeletalMeshComponent* Body;
	USkeletalMeshComponent* Emotion;
	USkeletalMeshComponent* Upper;
	USkeletalMeshComponent* Lower;
	USkeletalMeshComponent* Shoes;
};
