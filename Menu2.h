#ifndef menu2H
#define menu2H

#include "ObjectBuffer.h"
#include "Text.h"

struct stRotatePad
{
	int MainPadNumber; 
	int Current;
	vector<string> Strings;//rotate strings
};

class MMenu2
{
private:
	int* pWindowWidth;
    int* pWindowHeight;
	float* pCoordinateScale;
	
	bool CanNavigate;
	unsigned int PadsLimit;
	int CurrentPadNumber; //-1 if none selected
	vector<string> Strings;
	MText Pads;
	
	float FontSize;
	float FixX;
	float VerticalSpaceSize;
	float MenuTotalSize;
	float StartY;
	glm::vec2 ScreenCenter;
	float SymbolWidthCoeff;
	
	MObjectBuffer DecorBuffer;
	MObject* DecorObject;//0-left, 1-right
	unsigned int txDecorId;
	glm::vec2 DecorSize;
	
	vector<stRotatePad> RotatePads;
	
public:
	MMenu2();
	~MMenu2();
	bool Initialize(int* inpWindowWidth, int* inpWindowHeight, float* inpCoordinateScale, unsigned int inFontTextureId, float inFontSize, float inFixX, unsigned int intxDecorId, unsigned int inPadsLimit);
	bool AddPadMenu(const char* String);
	bool Resize();
	bool SelectPad(int PadNumber);
	void SelectPadByMouse(int x, int y);
	void SelectNextPad();
	void SelectPrevPad();
	int GetCurrentPadNumber();
	void Draw();
	void Close();
	void SetCanNavigate(bool inCanNavigate);
	
	void UpdateDecor();
	
	bool AddRotatePad(unsigned int MainPadNumber);
	bool AddRotatePadString(unsigned int RotatePadNumber, const char* String);
	bool AddRotatePadString(unsigned int RotatePadNumber, string String);
	int UseRotatePad(unsigned int RotatePadNumber);
	int GetRotatePadCurrentString(unsigned int RotatePadNumber);
	
	bool UpdatePad(unsigned int MainPadNumber, const char* String);
};

#endif
