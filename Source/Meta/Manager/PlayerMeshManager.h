// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Manager/Manager.h"
#include "Enum/PlayerMeshEnum.h"
#include "PlayerMeshManager.generated.h"

/**
 * 
 */
UCLASS()
class META_API UPlayerMeshManager : public UManager
{
	GENERATED_BODY()
	
public:
	DECLARE_MANAGER_GET_INSTANCE(UPlayerMeshManager);

public:
	UPlayerMeshManager();

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Setup();

public:
	USkeletalMesh*   GetBodyMesh  (int EPB)                   const;
	USkeletalMesh*   GetOutfitMesh(int EPB, int EPO, int Idx) const;
	UAnimationAsset* GetAnimation (int EPB, int Idx)          const;

public:
	const TArray<USkeletalMesh*>& GetFaceMeshs(int EPB) const;
	const TArray<USkeletalMesh*>& GetUpperMeshs(int EPB) const;
	const TArray<USkeletalMesh*>& GetLowerMeshs(int EPB) const;
	const TArray<USkeletalMesh*>& GetShoesMeshs(int EPB) const;

public:
	struct FAsset {
		USkeletalMesh*         Body;
		TArray<USkeletalMesh*> Outfit[EPO_OutfitCount];
		UAnimationAsset*       Anim[EPA_AnimCount];
	};
	FAsset Assets[EPB_BodyCount];
};