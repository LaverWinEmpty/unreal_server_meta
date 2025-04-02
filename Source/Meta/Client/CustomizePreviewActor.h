// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomizePreviewActor.generated.h"

class USkeletalMeshComponent;

UCLASS()
class META_API ACustomizePreviewActor : public AActor
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
	USkeletalMeshComponent* Body;
	USkeletalMeshComponent* Emotion;
	USkeletalMeshComponent* Outfit;
};
