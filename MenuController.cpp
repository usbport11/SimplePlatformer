#include "stdafx.h"
#include "MenuController.h"

MMenuController::MMenuController()
{
	pCurrentMenu = NULL;
	CurrentMenuNumber = 0;
	KeyToChange = -1;
}

MMenuController::~MMenuController()
{
	CurrentMenuNumber = 0;
}

bool MMenuController::SetCurrentMenu(unsigned char Number)
{
	switch(Number)
	{
		case MNU_MAIN:
			pCurrentMenu = &MainMenu;
			CurrentMenuNumber = Number;
			return true;
		case MNU_SETTINGS:
			pCurrentMenu = &SettingsMenu;
			CurrentMenuNumber = Number;
			return true;
		case MNU_CONTROLS:
			pCurrentMenu = &ControlsMenu;
			CurrentMenuNumber = Number;
			return true;
		case MNU_SOUND:
			pCurrentMenu = &SoundMenu;
			CurrentMenuNumber = Number;
			return true;
		case MNU_GAMEPLAY:
			pCurrentMenu = &GameplayMenu;
			CurrentMenuNumber = Number;
			return true;
		case MNU_GRAPHICS:
			pCurrentMenu = &GraphicsMenu;
			CurrentMenuNumber = Number;
			return true;
	}
	return false;
}

unsigned char MMenuController::GetCurrentMenuNumber()
{
	return CurrentMenuNumber;
}

int MMenuController::MenuKeyChange(WPARAM wParam, LPARAM lParam)
{
	if(KeyToChange < 0) return -1;
	cout<<wParam<<endl;
	char KeyName[32] = {0};
	string FullMenuString;
	switch(KeyToChange)
	{
		case 0:
			FullMenuString = "Move left: ";
			break;
		case 1:
			FullMenuString = "Move right: ";
			break;
		case 2:
			FullMenuString = "Jump: ";
			break;
		default:
			KeyToChange = -1;
			return KeyToChange;
	}
	GetKeyNameText(lParam, KeyName, 31);
	FullMenuString += KeyName;
	pCurrentMenu->UpdatePad(KeyToChange, FullMenuString.c_str());
	pCurrentMenu->SetCanNavigate(true);
	KeyToChange = -1;
	
	return KeyToChange;
}

void MMenuController::MenusProcess()
{
	switch(GetCurrentMenuNumber())
	{
		case MNU_MAIN:
			switch(pCurrentMenu->GetCurrentPadNumber())
			{
				case 0:
					OnMenuStart();
					return;
				case 1:
					SetCurrentMenu(MNU_SETTINGS);
					return;
				case 2:
					OnMenuThanks();
					return;
				case 3:
					OnMenuExit();
					return;
			}
			break;
		case MNU_SETTINGS:
			switch(pCurrentMenu->GetCurrentPadNumber())
			{
				case 0:
					SetCurrentMenu(MNU_GRAPHICS);
					return;
				case 1: 
					SetCurrentMenu(MNU_SOUND);
					return;
				case 2:
					SetCurrentMenu(MNU_GAMEPLAY);
					return;
				case 3:
					SetCurrentMenu(MNU_CONTROLS);
					return;
				case 4:
					SetCurrentMenu(MNU_MAIN);
					return;
			}
			break;
		case MNU_GRAPHICS:
			switch(pCurrentMenu->GetCurrentPadNumber())
			{
				case 0:
					pCurrentMenu->UseRotatePad(0);
					return;
				case 1:
					pCurrentMenu->UseRotatePad(1);
					return;
				case 2:
					OnMenuGraphicsAccept();
					return;
				case 3:
					SetCurrentMenu(MNU_SETTINGS);
					return;
			}
			break;
		case MNU_CONTROLS:
			switch(pCurrentMenu->GetCurrentPadNumber())
			{
				case 0:
					pCurrentMenu->SetCanNavigate(false);
					KeyToChange = 0;
					break;
				case 1:
					pCurrentMenu->SetCanNavigate(false);
					KeyToChange = 1;
					break;
				case 2:
					pCurrentMenu->SetCanNavigate(false);
					KeyToChange = 2;
					break;
				case 3:
					SetCurrentMenu(MNU_SETTINGS);
					return;
			}
			break;
		case MNU_SOUND:
			switch(pCurrentMenu->GetCurrentPadNumber())
			{
				case 0:
					pCurrentMenu->UseRotatePad(0);
					return;
				case 1:
					pCurrentMenu->UseRotatePad(1);
					return;
				case 2:
					OnMenuSoundAccept();
					return;
				case 3:
					SetCurrentMenu(MNU_SETTINGS);
					return;
			}
			break;
		case MNU_GAMEPLAY:
			switch(pCurrentMenu->GetCurrentPadNumber())
			{
				case 0:
					pCurrentMenu->UseRotatePad(0);
					return;
				case 1:
					pCurrentMenu->UseRotatePad(1);
					return;
				case 2:
					pCurrentMenu->UseRotatePad(2);
					return;
				case 3:
					OnMenuGameplayAccept();
					return;
				case 4:
					SetCurrentMenu(MNU_SETTINGS);
					return;
			}
			break;
	}
}

bool MMenuController::MenusInitialize(int* pWindowWidth, int* pWindowHeight, float* pCoordinateScale, unsigned int TextureFontId, unsigned int TexturePadId, vector<stResolution>* pResolutions)
{
	cout<<"Menu: "<<*pWindowWidth<<" "<<*pWindowHeight<<" "<<*pCoordinateScale<<endl;
	//main menu
	if(!MainMenu.Initialize(pWindowWidth, pWindowHeight, pCoordinateScale, TextureFontId, 0.44f, 0.20f, TexturePadId, 4)) return false;
	if(!MainMenu.AddPadMenu("Start")) return false;
	if(!MainMenu.AddPadMenu("Settings")) return false;
	if(!MainMenu.AddPadMenu("Credits")) return false;
	if(!MainMenu.AddPadMenu("Exit")) return false;
	if(!MainMenu.SelectPad(0)) return false;
	//settings menu
	if(!SettingsMenu.Initialize(pWindowWidth, pWindowHeight, pCoordinateScale, TextureFontId, 0.44f, 0.20f, TexturePadId, 5)) return false;
	if(!SettingsMenu.AddPadMenu("Graphics")) return false;
	if(!SettingsMenu.AddPadMenu("Sound")) return false;
	if(!SettingsMenu.AddPadMenu("Gameplay")) return false;
	if(!SettingsMenu.AddPadMenu("Controls")) return false;
	if(!SettingsMenu.AddPadMenu("Back")) return false;
	if(!SettingsMenu.SelectPad(4)) return false;
	if(!SettingsMenu.AddRotatePad(0)) return false;
	//graphics menu
	if(!GraphicsMenu.Initialize(pWindowWidth, pWindowHeight, pCoordinateScale, TextureFontId, 0.44f, 0.20f, TexturePadId, 4)) return false;
	if(!GraphicsMenu.AddPadMenu("800x600")) return false;
	if(!GraphicsMenu.AddPadMenu("Windowed")) return false;
	if(!GraphicsMenu.AddPadMenu("Accept")) return false;
	if(!GraphicsMenu.AddPadMenu("Back")) return false;
	if(!GraphicsMenu.SelectPad(3)) return false;
	if(!GraphicsMenu.AddRotatePad(0)) return false;
	string ResolutionString;
	char ConvertBuffer[5];
	for(unsigned int i=0; i<pResolutions->size(); i++)
	{
		memset(ConvertBuffer, 0, 5);
		ResolutionString = itoa(pResolutions->at(i).X, ConvertBuffer, 10);
		ResolutionString += "x";
		memset(ConvertBuffer, 0, 5);
		ResolutionString += itoa(pResolutions->at(i).Y, ConvertBuffer, 10);
		if(!GraphicsMenu.AddRotatePadString(0, ResolutionString)) return false;
	}
	if(!GraphicsMenu.AddRotatePad(1)) return false;
	if(!GraphicsMenu.AddRotatePadString(1, "Windowed")) return false;
	if(!GraphicsMenu.AddRotatePadString(1, "Fullscreen")) return false;
	if(!GraphicsMenu.SelectPad(3)) return false;
	//controls menu
	if(!ControlsMenu.Initialize(pWindowWidth, pWindowHeight, pCoordinateScale, TextureFontId, 0.44f, 0.20f, TexturePadId, 4)) return false;
	if(!ControlsMenu.AddPadMenu("Move left: Left")) return false;
	if(!ControlsMenu.AddPadMenu("Move right: Right")) return false;
	if(!ControlsMenu.AddPadMenu("Jump: Up")) return false;
	if(!ControlsMenu.AddPadMenu("Back")) return false;
	if(!ControlsMenu.SelectPad(3)) return false;
	//gameplay menu
	if(!GameplayMenu.Initialize(pWindowWidth, pWindowHeight, pCoordinateScale, TextureFontId, 0.44f, 0.20f, TexturePadId, 5)) return false;
	if(!GameplayMenu.AddPadMenu("Fog: Off")) return false;
	if(!GameplayMenu.AddPadMenu("Weather: Off")) return false;
	if(!GameplayMenu.AddPadMenu("Softcore: Off")) return false;
	if(!GameplayMenu.AddPadMenu("Accept")) return false;
	if(!GameplayMenu.AddPadMenu("Back")) return false;
	if(!GameplayMenu.AddRotatePad(0)) return false;
	if(!GameplayMenu.AddRotatePadString(0, "Fog: Off")) return false;
	if(!GameplayMenu.AddRotatePadString(0, "Fog: On")) return false;
	if(!GameplayMenu.AddRotatePad(1)) return false;
	if(!GameplayMenu.AddRotatePadString(1, "Weather: Off")) return false;
	if(!GameplayMenu.AddRotatePadString(1, "Weather: On")) return false;
	if(!GameplayMenu.AddRotatePad(2)) return false;
	if(!GameplayMenu.AddRotatePadString(2, "Softcore: Off")) return false;
	if(!GameplayMenu.AddRotatePadString(2, "Softcore: On")) return false;
	if(!GameplayMenu.SelectPad(3)) return false;
	//sound menu
	if(!SoundMenu.Initialize(pWindowWidth, pWindowHeight, pCoordinateScale, TextureFontId, 0.44f, 0.20f, TexturePadId, 4)) return false;
	if(!SoundMenu.AddPadMenu("Sound: Off")) return false;
	if(!SoundMenu.AddPadMenu("Volume: 1.0")) return false;
	if(!SoundMenu.AddPadMenu("Accept")) return false;
	if(!SoundMenu.AddPadMenu("Back")) return false;
	if(!SoundMenu.AddRotatePad(0)) return false;
	if(!SoundMenu.AddRotatePadString(0, "Sound: Off")) return false;
	if(!SoundMenu.AddRotatePadString(0, "Sound: On")) return false;
	if(!SoundMenu.AddRotatePad(1)) return false;
	if(!SoundMenu.AddRotatePadString(1, "Volume: 1.0")) return false;
	if(!SoundMenu.AddRotatePadString(1, "Volume: 0.9")) return false;
	if(!SoundMenu.AddRotatePadString(1, "Volume: 0.8")) return false;
	if(!SoundMenu.AddRotatePadString(1, "Volume: 0.7")) return false;
	if(!SoundMenu.AddRotatePadString(1, "Volume: 0.6")) return false;
	if(!SoundMenu.AddRotatePadString(1, "Volume: 0.5")) return false;
	if(!SoundMenu.AddRotatePadString(1, "Volume: 0.4")) return false;
	if(!SoundMenu.AddRotatePadString(1, "Volume: 0.3")) return false;
	if(!SoundMenu.AddRotatePadString(1, "Volume: 0.2")) return false;
	if(!SoundMenu.AddRotatePadString(1, "Volume: 0.1")) return false;
	if(!SoundMenu.SelectPad(2)) return false;
	
	pCurrentMenu = &MainMenu;
	
	return true;
}

void MMenuController::MenusResize()
{
	if(pCurrentMenu)
	{
		MainMenu.Resize();
		SettingsMenu.Resize();
		ControlsMenu.Resize();
		GameplayMenu.Resize();
		GraphicsMenu.Resize();
		SoundMenu.Resize();
	}
}

void MMenuController::MenusClose()
{
	pCurrentMenu = NULL;
	MainMenu.Close();
	SettingsMenu.Close();
	ControlsMenu.Close();
	GameplayMenu.Close();
	GraphicsMenu.Close();
	SoundMenu.Close();
}

void MMenuController::OnMenuStart(){}
void MMenuController::OnMenuThanks(){}
void MMenuController::OnMenuExit(){}
void MMenuController::OnMenuGraphicsAccept(){}
void MMenuController::OnMenuGameplayAccept(){}
void MMenuController::OnMenuSoundAccept(){}
