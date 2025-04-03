// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Manager/Manager.h"
#include "PlayerMeshManager.generated.h"

/**
 * 
 */
UCLASS()
class META_API UPlayerMeshManager : public UManager
{
	GENERATED_BODY()

public:
	// TODO: 이거로 refactor
	// 다른 에셋 추가 안 할거면 그대로
	enum EBodyPart {
		EBP_Face,
		EBP_Upper,
		EBP_Lower,
		EBP_Shoes,

		//
		EBP_Count
	};
	
public:
	DECLARE_MANAGER_GET_INSTANCE(UPlayerMeshManager);

public:
	UPlayerMeshManager();

public:
	USkeletalMesh*         BodyAsset;
	TArray<USkeletalMesh*> EmotionAssets;
	TArray<USkeletalMesh*> UpperAssets;
	TArray<USkeletalMesh*> LowerAssets;
	TArray<USkeletalMesh*> ShoesAssets;

public:
	UAnimationAsset* IdleAnimationAsset;
};
