// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/PlayerMeshManager.h"

UPlayerMeshManager::UPlayerMeshManager() {
	static const FString AnimsPath  = _T("/Game/Creative_Characters_FREE/Animations/");
	static const FString MeshesPath = _T("/Game/Creative_Characters_FREE/Skeleton_Meshes/");

	// body name
	static const FString BodyNames[EPB_BodyCount]{
		_T("SK_Body_010.SK_Body_010"),
	};

	// anim names
	static const FString AnimNames[EPB_BodyCount][EPA_AnimCount]{
		/* Body Type 1 */ {
			_T("ANIM_Idle_Breathing.ANIM_Idle_Breathing"),
		}
	};

	// mesh names
	static const TArray<FString> MeshNames[EPB_BodyCount][EPO_OutfitCount] = {
		/* Body Type 1 */ {
			/* Face */ {
				_T("SK_Male_emotion_usual_001.SK_Male_emotion_usual_001"),
				_T("SK_Male_emotion_happy_002.SK_Male_emotion_happy_002"),
				_T("SK_Male_emotion_angry_003.SK_Male_emotion_angry_003"),
			},

			/* Upper */ {
				_T("SK_Outerwear_029.SK_Outerwear_029"),
				_T("SK_Outerwear_036.SK_Outerwear_036"),
			},

			/* Lower */ {
				_T("SK_Pants_010.SK_Pants_010"),
				_T("SK_Pants_014.SK_Pants_014"),
			},

			/* Shoes */ {
				_T("SK_Shoe_Slippers_002.SK_Shoe_Slippers_002"),
				_T("SK_Shoe_Slippers_005.SK_Shoe_Slippers_005"),
				_T("SK_Shoe_Sneakers_009.SK_Shoe_Sneakers_009"),
			},
		}
	};

	static TArray<ConstructorHelpers::FObjectFinder<USkeletalMesh>>   BodyFinder;
	static TArray<ConstructorHelpers::FObjectFinder<USkeletalMesh>>   MeshFinder[EPB_BodyCount][EPO_OutfitCount];
	static TArray<ConstructorHelpers::FObjectFinder<UAnimationAsset>> AnimFinder[EPB_BodyCount];

	static bool bLoaded= false;
	if (!bLoaded) {
		bLoaded = true;

		for (int i = 0; i < EPB_BodyCount; ++i) {
			BodyFinder.Add(*(MeshesPath + BodyNames[i]));
		}

		for (int i = 0; i < EPB_BodyCount; ++i) {
			// find mesh object
			for (int j = 0; j < EPO_OutfitCount; ++j) {
				int loop = MeshNames[i][j].Num(); // get num of outfit parts
				for (int k = 0; k < loop; ++k) {
					MeshFinder[i][j].Add(*(MeshesPath + MeshNames[i][j][k])); // find
				}
			}

			// find anim object
			AnimFinder[i].Reserve(EPA_AnimCount);
			for (int j = 0; j < EPA_AnimCount; ++j) {
				AnimFinder[i].Add(*(AnimsPath + AnimNames[i][j])); // find
			}
		}

	}

	for (int i = 0; i < EPB_BodyCount; ++i) {
		Assets[i].Body = BodyFinder[i].Object;

		// get mesh object
		for (int j = 0; j < EPO_OutfitCount; ++j) {
			int loop = MeshFinder[i][j].Num();
			for (int k = 0; k < loop; ++k) {
				Assets[i].Outfit[j].Add(MeshFinder[i][j][k].Object); // get
			}
		}

		// get anim object
		for (int j = 0; j < EPA_AnimCount; ++j) {
			Assets[i].Anim[j] = AnimFinder[i][j].Object; // get
		}
	}

	// check
	for (int i = 0; i < EPB_BodyCount; ++i) {
		for (int j = 0; j < EPO_OutfitCount; ++j) {
			checkf(MeshFinder[i][j].Num(), _T("At least 1 asset is required.: Body[%d] Outfit[%d]"), i, j);
		}

		checkf(Assets[i].Body, _T("Load Failed: Body[%d]"), i);

		for (int j = 0; j < EPO_OutfitCount; ++j) {
			int loop = Assets[i].Outfit[j].Num();
			for (int k = 0; k < loop; ++k) {
				checkf(Assets[i].Outfit[j][k], _T("Load Failed: Body[%d] Outfit[%d] Number[%d]"), i, j, k);
			}
		}

		for (int j = 0; j < EPA_AnimCount; ++j) {
			checkf(Assets[i].Anim[j], _T("Load Failed: Body[%d] Animation[%d]"), i, j);
		}
	}
}

USkeletalMesh* UPlayerMeshManager::GetBodyMesh(int Type) const {
	return Assets[Type].Body;
}

USkeletalMesh* UPlayerMeshManager::GetOutfitMesh(int Type, int Part, int Index) const {
	return Assets[Type].Outfit[Part][Index];
}

UAnimationAsset* UPlayerMeshManager::GetAnimation(int Type, int Index) const {
	return Assets[Type].Anim[Index];
}

const TArray<USkeletalMesh*>& UPlayerMeshManager::GetFaceMeshs(int In) const {
	return Assets[In].Outfit[EPO_Face];
}

const TArray<USkeletalMesh*>& UPlayerMeshManager::GetUpperMeshs(int In) const {
	return Assets[In].Outfit[EPO_Upper];
}

const TArray<USkeletalMesh*>& UPlayerMeshManager::GetLowerMeshs(int In) const {
	return Assets[In].Outfit[EPO_Lower];
}

const TArray<USkeletalMesh*>& UPlayerMeshManager::GetShoesMeshs(int In) const {
	return Assets[In].Outfit[EPO_Shoes];
}
