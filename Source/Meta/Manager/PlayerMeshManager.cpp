// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/PlayerMeshManager.h"

UPlayerMeshManager::UPlayerMeshManager() {}

void UPlayerMeshManager::Setup() {
	if (!UManager::IsUser(this)) {
		return; // 사용자만
	}

	// nullptr allows list
	static TSet<int> Nullable = {
		EPL_Hair,
		EPL_Upper,
		EPL_Lower,
		EPL_Shoes,
	};

	static const FString AnimsPath  = _T("/Game/Creative_Characters_FREE/Animations/");
	static const FString MeshesPath = _T("/Game/Creative_Characters_FREE/Skeleton_Meshes/");

	// body name
	static const FString BodyNames[EPB_Count]{
		_T("SK_Body_010.SK_Body_010"),
	};

	// anim names
	static const FString AnimNames[EPB_Count][EPA_Count]{
		/* Body Type 1 */ {
			_T("ANIM_Idle_Breathing.ANIM_Idle_Breathing"),
		}
	};

	// mesh names
	static const TArray<FString> MeshNames[EPB_Count][EPL_Count] = {
		/* Body Type 1 */ {
			/* Face */ {
				_T("SK_Male_emotion_usual_001.SK_Male_emotion_usual_001"),
				_T("SK_Male_emotion_happy_002.SK_Male_emotion_happy_002"),
				_T("SK_Male_emotion_angry_003.SK_Male_emotion_angry_003"),
			},

			/* Hair */ {
				_T("SK_Hairstyle_male_010.SK_Hairstyle_male_010"),
				_T("SK_Hairstyle_male_012.SK_Hairstyle_male_012"),
			},

			/* Upper */ {
				_T("SK_Outerwear_029.SK_Outerwear_029"),
				_T("SK_Outerwear_036.SK_Outerwear_036"),
				_T("SK_T_Shirt_009.SK_T_Shirt_009"),
			},

			/* Lower */ {
				_T("SK_Pants_010.SK_Pants_010"),
				_T("SK_Pants_014.SK_Pants_014"),
				_T("SK_Shorts_003.SK_Shorts_003"),
			},

			/* Shoes */ {
				_T("SK_Shoe_Slippers_002.SK_Shoe_Slippers_002"),
				_T("SK_Shoe_Slippers_005.SK_Shoe_Slippers_005"),
				_T("SK_Shoe_Sneakers_009.SK_Shoe_Sneakers_009"),
			},
		}
	};

	auto LoadBodyMesh = [](int i) {
		return Cast<USkeletalMesh>(
			StaticLoadObject(
				USkeletalMesh::StaticClass(),
				nullptr,
				*(MeshesPath + BodyNames[i])
			)
		);
	};

	auto LoadOutfitMesh = [](int i, int j, int k) {
		return Cast<USkeletalMesh>(
			StaticLoadObject(
				USkeletalMesh::StaticClass(),
				nullptr,
				*(MeshesPath + MeshNames[i][j][k])
			)
		);
	};

	auto LoadAnimation = [](int i, int j) {
		return Cast<UAnimationAsset>(
			StaticLoadObject(
				UAnimationAsset::StaticClass(),
				nullptr,
				*(AnimsPath + AnimNames[i][j])
			)
		);
	};

	// get assets
	for (int i = 0; i < EPB_Count; ++i) {
		Assets[i].Body = LoadBodyMesh(i);

		// get mesh object
		for (int j = 0; j < EPL_Count; ++j) {
			if (Nullable.Find(j)) {
				Assets[i].LookMesh[j].Add(nullptr); // nullable
			}

			// load
			int Loop = MeshNames[i][j].Num();
			for (int k = 0; k < Loop; ++k) {
				Assets[i].LookMesh[j].Add(LoadOutfitMesh(i, j, k)); // get
			}
		}

		// load anim object
		for (int j = 0; j < EPA_Count; ++j) {
			Assets[i].Anim[j] = LoadAnimation(i, j); // get
		}
	}

	// check
	for (int i = 0; i < EPB_Count; ++i) {
		checkf(Assets[i].Body, _T("Load Failed: Body[%d]"), i);

		for (int j = 0; j < EPL_Count; ++j) {
			if (!Nullable.Find(j)) {
				checkf(MeshNames[i][j].Num(), _T("At least 1 asset is required.: Body[%d] Outfit[%d]"), i, j);
			}
		}

		for (int j = 0; j < EPL_Count; ++j) {
			int Loop = Assets[i].LookMesh[j].Num();
			// [0] check nullable
			if (Assets[i].LookMesh[j][0] == nullptr && !Nullable.Find(j)) {
				checkf(false, _T("Load Failed: Check Nullable -> Body[%d] Outfit[%d] Number[0]"), i, j);
			}
			// others
			for (int k = 1; k < Loop; ++k) {
				checkf(Assets[i].LookMesh[j][k], _T("Load Failed: Body[%d] Outfit[%d] Number[%d]"), i, j, k);
			}
		}

		for (int j = 0; j < EPA_Count; ++j) {
			checkf(Assets[i].Anim[j], _T("Load Failed: Body[%d] Animation[%d]"), i, j);
		}
	}
}

void UPlayerMeshManager::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);
	Setup();
}

USkeletalMesh* UPlayerMeshManager::GetBodyMesh(int Type) const {
	return Assets[Type].Body;
}

USkeletalMesh* UPlayerMeshManager::GetOutfitMesh(int Type, int Part, int Index) const {
	return Assets[Type].LookMesh[Part][Index];
}

UAnimationAsset* UPlayerMeshManager::GetAnimation(int Type, int Index) const {
	return Assets[Type].Anim[Index];
}

const TArray<USkeletalMesh*>& UPlayerMeshManager::GetFaceMeshs(int In) const {
	return Assets[In].LookMesh[EPL_Face];
}

const TArray<USkeletalMesh*>& UPlayerMeshManager::GetUpperMeshs(int In) const {
	return Assets[In].LookMesh[EPL_Upper];
}

const TArray<USkeletalMesh*>& UPlayerMeshManager::GetLowerMeshs(int In) const {
	return Assets[In].LookMesh[EPL_Lower];
}

const TArray<USkeletalMesh*>& UPlayerMeshManager::GetShoesMeshs(int In) const {
	return Assets[In].LookMesh[EPL_Shoes];
}
