#ifndef menucontrollerH
#define menucontrollerH

#include "Menu2.h"

#define MNU_MAIN 0
#define MNU_SETTINGS 1
#define MNU_CONTROLS 2
#define MNU_GRAPHICS 3
#define MNU_SOUND 4
#define MNU_GAMEPLAY 5

class MMenuController
{
private:
	int KeyToChange;
	unsigned char CurrentMenuNumber;
	MMenu2 MainMenu;
	MMenu2 SettingsMenu;
	MMenu2 ControlsMenu;
	MMenu2 GameplayMenu;
	MMenu2 GraphicsMenu;
	MMenu2 SoundMenu;
protected:
	MMenu2* pCurrentMenu;
	virtual void OnMenuStart();
	virtual void OnMenuThanks();
	virtual void OnMenuExit();
	virtual void OnMenuGraphicsAccept();
	virtual void OnMenuGameplayAccept();
	virtual void OnMenuSoundAccept();
public:
	MMenuController();
	~MMenuController();
	bool MenusInitialize(int* pWindowWidth, int* pWindowHeight, float* pCoordinateScale, unsigned int TextureFontId, unsigned int TexturePadId, vector<stResolution>* pResolutions);
	void MenusClose();
	void MenusProcess();
	void MenusResize();
	bool SetCurrentMenu(unsigned char Number);
	unsigned char GetCurrentMenuNumber();
	int MenuKeyChange(WPARAM wParam, LPARAM lParam);
};

#endif
