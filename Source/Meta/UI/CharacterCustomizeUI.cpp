#include "UI/CharacterCustomizeUI.h"
#include "Components/Button.h"
#include "Lobby/LobbyHandler.h"

void UCharacterCustomizeUI::Bind(ALobbyHandler* In) {
	FaceNextButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnFaceNext);
	FacePrevButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnFacePrev);
	HairNextButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnHairNext);
	HairPrevButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnHairPrev);
	UpperNextButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnUpperNext);
	UpperPrevButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnUpperPrev);
	LowerNextButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnLowerNext);
	LowerPrevButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnLowerPrev);
	ShoesNextButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnShoesNext);
	ShoesPrevButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnShoesPrev);
	EnterButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnCustomEnd);
	ExitButton->OnClicked.AddDynamic(In, &ALobbyHandler::OnCustomCancel);
}
