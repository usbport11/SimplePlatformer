#include "stdafx.h"
#include "Menu2.h"

MMenu2::MMenu2()
{
	pWindowWidth = NULL;
	pWindowHeight = NULL;
	pCoordinateScale = NULL;
	VerticalSpaceSize = 0;
	ScreenCenter = glm::vec2(0, 0);
	FontSize = 0;
	FixX = 0;
	MenuTotalSize = 0;
	StartY = 0;
	
	PadsLimit = 0;
	CurrentPadNumber = -1;
	
	DecorObject = NULL;
	
	CanNavigate = true;
	DecorSize = glm::vec2(0.32f, 0.32f);
}

MMenu2::~MMenu2()
{
	pWindowWidth = NULL;
	pWindowHeight = NULL;
	pCoordinateScale = NULL;
}

bool MMenu2::Initialize(int* inpWindowWidth, int* inpWindowHeight, float* inpCoordinateScale, unsigned int inFontTextureId, float inFontSize, float inFixX, unsigned int intxDecorId, unsigned int inPadsLimit)
{
	if(!inpWindowWidth || !inpWindowHeight || !inpCoordinateScale)
	{
		LogFile<<"Menu: Window size or scale is empty"<<endl;
		return false;
	}
	if(!inPadsLimit)
	{
		LogFile<<"Menu: Null menu size"<<endl;
		return false;
	}
	if(!intxDecorId)
	{
		LogFile<<"Menu: Decor texture is NULL"<<endl;
		return false;
	}
	txDecorId = intxDecorId;
	
	if(!Pads.Initialize(inFontTextureId, inFontSize, inFixX)) return false;
	
	FontSize = inFontSize;
	FixX = inFixX;
	pWindowWidth = inpWindowWidth;
	pWindowHeight = inpWindowHeight;
	pCoordinateScale = inpCoordinateScale;
	PadsLimit = inPadsLimit;
	
	VerticalSpaceSize = FixX;
	MenuTotalSize = (FontSize + VerticalSpaceSize) * PadsLimit;
	SymbolWidthCoeff = (FontSize - FixX) * 0.5;
	ScreenCenter = glm::vec2((*pWindowWidth * *pCoordinateScale) * 0.5, (*pWindowHeight * *pCoordinateScale) * 0.5);
	StartY = MenuTotalSize * 0.5 + ScreenCenter.y;
	
	if(!DecorBuffer.Initialize(GL_STREAM_DRAW)) return false;
	DecorObject = new MObject[2];
	if(!DecorBuffer.AddObject(&DecorObject[0], - DecorSize.x * 2, - DecorSize.y * 2, DecorSize.x, DecorSize.y, 0, 0, 1, 1, txDecorId)) return false;
	if(!DecorBuffer.AddObject(&DecorObject[1], - DecorSize.x * 2, - DecorSize.y * 2, DecorSize.x, DecorSize.y, 0, 0, 1, 1, txDecorId)) return false;
	DecorObject[0].FlipTextureByX();
	DecorBuffer.DisposeAll();
	
	return true;
}

bool MMenu2::AddPadMenu(const char* String)
{
	if(!String)
	{
		LogFile<<"Menu: Can't add empty string"<<endl;
		return false;
	}
	if(Strings.size() >= PadsLimit)
	{
		LogFile<<"Menu: Wrong pad number"<<endl;
		return false;
	}
	
	float StartX = ScreenCenter.x - strlen(String) * SymbolWidthCoeff;
	if(!Pads.AddString(StartX, StartY - (FontSize + VerticalSpaceSize) * Strings.size(), (char*)String)) return false;
	Strings.push_back(String);
	
	return true;
}

bool MMenu2::Resize()
{
	float StartX;
	ScreenCenter = glm::vec2((*pWindowWidth * *pCoordinateScale) * 0.5, (*pWindowHeight * *pCoordinateScale) * 0.5);
	StartY = MenuTotalSize * 0.5 + ScreenCenter.y;
	
	for(unsigned int i=0; i<Strings.size(); i++)
	{
		StartX = ScreenCenter.x - Strings[i].length() * SymbolWidthCoeff;
		if(!Pads.UpdateString(i, StartX, StartY - (FontSize + VerticalSpaceSize) * i, (char*)Strings[i].c_str())) return false;
	}
	UpdateDecor();

	return true;
}

bool MMenu2::SelectPad(int PadNumber)
{
	if(PadNumber < 0 || PadNumber > Strings.size())
	{
		LogFile<<"Menu2: Wrong Pad number"<<endl;
		return false;
	}
	glm::vec2 RegionPoint[2];
	stQuad Quad;
	Pads.GetStringRegion(PadNumber, RegionPoint[0], RegionPoint[1]);
	RegionPoint[0].x -= FixX;
	RegionPoint[1].x -= FixX;
	SetQuad(Quad, RegionPoint[0].x - DecorSize.x, RegionPoint[0].y + DecorSize.y * 0.5f, DecorSize.x, DecorSize.y);
	DecorObject[0].SetVertex(Quad);
	SetQuad(Quad, RegionPoint[1].x, RegionPoint[0].y + DecorSize.y * 0.5f, DecorSize.x, DecorSize.y);
	DecorObject[1].SetVertex(Quad);
	DecorBuffer.UpdateAll();
	CurrentPadNumber = PadNumber;
	
	return true;
}

void MMenu2::SelectPadByMouse(int x, int y)
{
	if(!CanNavigate) return;
	//convert mouse coordiantes to game coordinates
	stQuad Quad;
	glm::vec2 MouseGameCoord = glm::vec2(x * *pCoordinateScale, (*pWindowHeight - y) * *pCoordinateScale);
	glm::vec2 RegionPoint[2];
	
	for(unsigned int i=0; i<Strings.size(); i++)
	{
		Pads.GetStringRegion(i, RegionPoint[0], RegionPoint[1]);
		RegionPoint[0].x -= FixX;
		RegionPoint[1].x -= FixX;
		if(MouseGameCoord.x > RegionPoint[0].x &&  MouseGameCoord.x < RegionPoint[1].x && MouseGameCoord.y > RegionPoint[0].y && MouseGameCoord.y < RegionPoint[1].y)
		{
			SetQuad(Quad, RegionPoint[0].x - DecorSize.x, RegionPoint[0].y + DecorSize.y * 0.5f, DecorSize.x, DecorSize.y);
			DecorObject[0].SetVertex(Quad);
			SetQuad(Quad, RegionPoint[1].x, RegionPoint[0].y + DecorSize.y * 0.5f, DecorSize.x, DecorSize.y);
			DecorObject[1].SetVertex(Quad);
			DecorBuffer.UpdateAll();
			CurrentPadNumber = i;
			return;
		}
	}
}

void MMenu2::SelectNextPad()
{
	if(!CanNavigate) return;
	int PadNumber = CurrentPadNumber + 1;
	if(PadNumber >= Strings.size()) PadNumber = 0;
	SelectPad(PadNumber);
}
void MMenu2::SelectPrevPad()
{
	if(!CanNavigate) return;
	int PadNumber = CurrentPadNumber - 1;
	if(PadNumber < 0) PadNumber = Strings.size() - 1;
	SelectPad(PadNumber);
}

int MMenu2::GetCurrentPadNumber()
{
	return CurrentPadNumber;
}

void MMenu2::Draw()
{
	DecorBuffer.DrawAll();
	Pads.DrawAll();
}

void MMenu2::Close()
{
	Pads.Close();
	Strings.clear();
	if(DecorObject) delete [] DecorObject;
	DecorBuffer.Close();
	
	for(unsigned int i=0; i<RotatePads.size(); i++)
		RotatePads[i].Strings.clear();
	RotatePads.clear();
}

void MMenu2::SetCanNavigate(bool inCanNavigate)
{
	CanNavigate = inCanNavigate;
}

void MMenu2::UpdateDecor()
{
	stQuad Quad;
	glm::vec2 RegionPoint[2];
	Pads.GetStringRegion(CurrentPadNumber, RegionPoint[0], RegionPoint[1]);
	RegionPoint[0].x -= FixX;
	RegionPoint[1].x -= FixX;
	SetQuad(Quad, RegionPoint[0].x - DecorSize.x, RegionPoint[0].y + DecorSize.y * 0.5f, DecorSize.x, DecorSize.y);
	DecorObject[0].SetVertex(Quad);
	SetQuad(Quad, RegionPoint[1].x, RegionPoint[0].y + DecorSize.y * 0.5f, DecorSize.x, DecorSize.y);
	DecorObject[1].SetVertex(Quad);
	DecorBuffer.UpdateAll();
}

bool MMenu2::AddRotatePad(unsigned int MainPadNumber)
{
	stRotatePad RotatePad;
	RotatePad.Current = -1;
	RotatePad.MainPadNumber = MainPadNumber;
	RotatePads.push_back(RotatePad);
	
	return true;
}

bool MMenu2::AddRotatePadString(unsigned int RotatePadNumber, const char* String)
{
	if(!strlen(String)) 
	{
		LogFile<<"Menu: Empty string in rotate pad"<<endl;
		return false;
	}
	if(RotatePadNumber >= Strings.size())
	{
		LogFile<<"Menu: Wrong rotate pad number: too few main pads"<<endl;
		return false;
	}
	if(RotatePadNumber >= RotatePads.size())
	{
		LogFile<<"Menu: Wrong rotate pad number: too few rotate pads"<<endl;
		return false;
	}
	
	RotatePads[RotatePadNumber].Strings.push_back(String);
	if(RotatePads[RotatePadNumber].Strings.size() == 1) RotatePads[RotatePadNumber].Current = 0;
	
	return true;
}

bool MMenu2::AddRotatePadString(unsigned int RotatePadNumber, string String)
{
	if(!String.length()) 
	{
		LogFile<<"Menu: Empty string in rotate pad"<<endl;
		return false;
	}
	if(RotatePadNumber >= Strings.size())
	{
		LogFile<<"Menu: Wrong rotate pad number: too few main pads"<<endl;
		return false;
	}
	if(RotatePadNumber >= RotatePads.size())
	{
		LogFile<<"Menu: Wrong rotate pad number: too few rotate pads"<<endl;
		return false;
	}
	
	RotatePads[RotatePadNumber].Strings.push_back(String);
	if(RotatePads[RotatePadNumber].Strings.size() == 1) RotatePads[RotatePadNumber].Current = 0;
	
	return true;
}

int MMenu2::UseRotatePad(unsigned int RotatePadNumber)
{
	int MainPadNumber = RotatePads[RotatePadNumber].MainPadNumber;
	
	//check
	if(MainPadNumber < 0 || MainPadNumber >= Strings.size()) return -1;
	if(!RotatePads[RotatePadNumber].Strings.size()) return -1;
	//rotate
	RotatePads[RotatePadNumber].Current ++;
	if(RotatePads[RotatePadNumber].Current >= RotatePads[RotatePadNumber].Strings.size()) RotatePads[RotatePadNumber].Current = 0;
	//update main pad string
	Strings[MainPadNumber] = RotatePads[RotatePadNumber].Strings[RotatePads[RotatePadNumber].Current];
	float StartX = ScreenCenter.x - strlen(Strings[MainPadNumber].c_str()) * SymbolWidthCoeff;
	if(!Pads.UpdateString(MainPadNumber, StartX, StartY - (FontSize + VerticalSpaceSize) * MainPadNumber, (char*)Strings[MainPadNumber].c_str())) return -1;//SUSPISIOUS
	//update decor
	UpdateDecor();
	
	return RotatePads[RotatePadNumber].Current;
}

int MMenu2::GetRotatePadCurrentString(unsigned int RotatePadNumber)
{
	if(RotatePads.size() <= RotatePadNumber) return -1;
	if(!RotatePads[RotatePadNumber].Strings.size()) return -1;
	return RotatePads[RotatePadNumber].Current;
}

bool MMenu2::UpdatePad(unsigned int MainPadNumber, const char* String)
{
	if(MainPadNumber >= Strings.size()) return false;
	if(!String) return false;
	
	Strings[MainPadNumber] = String;
	float StartX = ScreenCenter.x - Strings[MainPadNumber].length() * SymbolWidthCoeff;
	if(!Pads.UpdateString(MainPadNumber, StartX, StartY - (FontSize + VerticalSpaceSize) * MainPadNumber, (char*)String)) return false;
	UpdateDecor();
	
	return true;
}
