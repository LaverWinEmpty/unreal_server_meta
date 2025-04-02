// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/LobbyMode.h"
#include "Client/CustomizePreviewActor.h"

ALobbyMode::ALobbyMode() {
	// 메시 경로
	static const FString Path = _T("/Game/Creative_Characters_FREE/Skeleton_Meshes/");

	// anim
	static ConstructorHelpers::FObjectFinder<UAnimationAsset> IdleFinder {
		_T("/Game/Creative_Characters_FREE/Animations/ANIM_Idle_Relaxed.ANIM_Idle_Relaxed")
	};
	check(IdleFinder.Object);
	IdleAnimationAsset = IdleFinder.Object;

	// find
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> EmotionFinder[] {
		*(Path + _T("SK_Male_emotion_usual_001.SK_Male_emotion_usual_001")),
		*(Path + _T("SK_Male_emotion_happy_002.SK_Male_emotion_happy_002")),
		*(Path + _T("SK_Male_emotion_angry_003.SK_Male_emotion_angry_003")),
	};
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> UpperFinder[] {
		*(Path + _T("SK_Outerwear_029.SK_Outerwear_029")),
		*(Path + _T("SK_Outerwear_036.SK_Outerwear_036")),
	};
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> LowerFinder[]{
		*(Path + _T("SK_Pants_010.SK_Pants_010")),
		*(Path + _T("SK_Pants_014.SK_Pants_014"))
	};

	// get
	for (int i = 0; i < sizeof(EmotionFinder) / sizeof(*EmotionFinder); ++i) {
		check(EmotionFinder[i].Object);
		EmotionAssets.Add(EmotionFinder[i].Object);
	}
	for (int i = 0; i < sizeof(UpperFinder) / sizeof(*UpperFinder); ++i) {
		check(UpperFinder[i].Object);
		UpperAssets.Add(UpperFinder[i].Object);
	}
	for (int i = 0; i < sizeof(LowerFinder) / sizeof(*LowerFinder); ++i) {
		check(LowerFinder[i].Object);
		LowerAssets.Add(LowerFinder[i].Object);
	}
}

void ALobbyMode::BeginPlay() {
	// TODO:
	PreviewActor = GetWorld()->SpawnActor<ACustomizePreviewActor>();

	
}
