// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyMode.generated.h"


UCLASS()
class META_API ALobbyMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ALobbyMode();

public:
	void BeginPlay() override;

private:
	AActor* PreviewActor;

private:
	/*UAnimationAsset* */
	UAnimationAsset* IdleAnimationAsset;
	
private:
	TArray<USkeletalMesh*> EmotionAssets;
	TArray<USkeletalMesh*> UpperAssets;
	TArray<USkeletalMesh*> LowerAssets;
	TArray<USkeletalMesh*> ShoesAssets;

private:
	TArray<TObjectPtr<USkeletalMeshComponent>> Emotions;
	TArray<TObjectPtr<USkeletalMeshComponent>> Uppers;
	TArray<TObjectPtr<USkeletalMeshComponent>> Lowers;
	TArray<TObjectPtr<USkeletalMeshComponent>> Shoes;
};
