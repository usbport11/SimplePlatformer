#include "stdafx.h"
#include "TextController.h"

MTextController::MTextController()
{
	HintNumber = 0;
}

MTextController::~MTextController()
{
	HintNumber = 0;
}

bool MTextController::Initialize(float Width, float Height, unsigned int TextureId)
{
	if(!TextureId) return false;
	
	//game strings
	GameStrings.push_back("Press 'E' to proceed to next level");//need extend function (center this time)
	GameStrings.push_back("Press 'Space' to begin");
	GameStrings.push_back("Press 'Space' to start a new game or 'ESC' for Menu");
	if(!txtGame.Initialize(TextureId, 0.28, 0.20)) return false;
	if(!txtGame.AddStringAndCenterByX(Width, Height - 1, (char*)GameStrings[0].c_str())) return false;
	for(unsigned int i=1; i<GameStrings.size(); i++)
	{
		if(!txtGame.AddStringAndCenterByX(Width, 0.5, (char*)GameStrings[i].c_str())) return false;
	}
	
	//text hints
	HintsStrings.push_back("Try reach exit and collect items as fast as possible");
	HintsStrings.push_back("Do not touch the liquid");
	HintsStrings.push_back("Take buffs and do not take debuffs");
	HintsStrings.push_back("For set pause - click mouse outside of game window");
	HintsStrings.push_back("Level statistics wrote in top left corner");
	HintsStrings.push_back("Sorry, but this is an empty hint");
	HintsStrings.push_back("Do not be so sirious - this is just a game");
	if(!txtHints.Initialize(TextureId, 0.32, 0.22)) return false;
	for(unsigned int i=0; i<HintsStrings.size(); i++)
	{
		if(!txtHints.AddStringAndCenterByX(Width, 1, (char*)HintsStrings[i].c_str())) return false;
	}
	
	//thanks
	ThanksStrings.push_back("CODE:            ");//=========CODE=========
	ThanksStrings.push_back("NektoCtulhu ");
	ThanksStrings.push_back("MUSIC:           ");//=========MUSIC=========
	ThanksStrings.push_back("Dr_Ago      ");
	ThanksStrings.push_back("TESTERS:         ");//=========TESTERS=========
	ThanksStrings.push_back("DenSver     ");
	ThanksStrings.push_back("KoreanNoName");
	ThanksStrings.push_back("alexxx_one  ");
	ThanksStrings.push_back("G-Rey       ");
	ThanksStrings.push_back("Fikset      ");
	ThanksStrings.push_back("EzZtel      ");
	ThanksStrings.push_back("Servis23    ");
	ThanksStrings.push_back("THANKS:          ");//=========THANKS=========
	ThanksStrings.push_back("VikinG      ");
	ThanksStrings.push_back("LOSTED:          ");//=========LOSTED=========
	ThanksStrings.push_back("Marya       ");
	if(!txtThanks.Initialize(TextureId, 0.32, 0.22)) return false;
	for(unsigned int i=0; i<ThanksStrings.size(); i++)
	{
		if(!txtThanks.AddStringAndCenterByX(Width, Height - 0.5f - i * 0.3f, (char*)ThanksStrings[i].c_str())) return false;
	}
	
	return true;
}

bool MTextController::Size(float Width, float Height)
{
	if(!GameStrings.size()) return false;
	if(!txtGame.UpdateStringAndCenterByX(0, Width, Height - 1, (char*)GameStrings[0].c_str())) return false;
	for(unsigned int i=1; i<GameStrings.size(); i++)
	{
		if(!txtGame.UpdateStringAndCenterByX(i, Width, 0.5, (char*)GameStrings[i].c_str())) return false;
	}
	for(unsigned int i=0; i<HintsStrings.size(); i++)
	{
		if(!txtHints.UpdateStringAndCenterByX(i, Width, 1, (char*)HintsStrings[i].c_str())) return false;
	}
	for(unsigned int i=0; i<ThanksStrings.size(); i++)
	{
		if(!txtThanks.UpdateStringAndCenterByX(i, Width, Height - 0.5f - i * 0.3f, (char*)ThanksStrings[i].c_str())) return false;
	}

	return true;
}

void MTextController::SetRandomHint()
{
	HintNumber = rand() % HintsStrings.size();
}

void MTextController::DrawHint()
{
	txtHints.DrawString(HintNumber);
}

void MTextController::DrawGameString(unsigned int Number)
{
	if(Number >= GameStrings.size()) return;
	txtGame.DrawString(Number);
}

void MTextController::DrawThanks()
{
	txtThanks.DrawAll();
}

void MTextController::Close()
{
	GameStrings.clear();
	HintsStrings.clear();
	ThanksStrings.clear();
	txtGame.Close();
	txtHints.Close();
	txtThanks.Close();
}
