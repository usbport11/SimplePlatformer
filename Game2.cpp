#include "stdafx.h"
#include "Game2.h"

MGame2::MGame2():MWindow(),MMenuController()
{
	Pause = true;
    Key = new bool [256];
    memset(Key, 0, 256);
	
	pDrawFunc = NULL;
	
	txWait = NULL;
	txLogo = NULL;
	txLose = NULL;
	txMenuPad = NULL;
	txFont = NULL;
	txMenuPad = NULL;
	txBuffs = NULL;

	CameraRound = 1000;
	CoordinateScale = 0.01; //1 metre - 100 pixels
	
	BlinkEnable = true;
	
	KeyToChange = -1;
	CustomKeys[0] = VK_LEFT;
	CustomKeys[1] = VK_RIGHT;
	CustomKeys[2] = VK_UP;
	
	ViewScale = 1.0f;
	ViewOffset = -1.92f;
	ViewCenter = glm::vec2(0.0f, 0.0f);
	
	LogoQuad = NULL;
	WaitQuad = NULL;
	LoseQuad = NULL;
	
	SoundEnabled = false;
	sndLevel = NULL;
	sndPickup = NULL;
	sndMenuSelect = NULL;
	sndMenuClick = NULL;
}

MGame2::~MGame2()
{
	pDrawFunc = NULL;
}

bool MGame2::Initialize(HINSTANCE hInstance)
{	
	if(!CreateMainWindow(hInstance)) return 0;

    LogFile<<"Game2: Initialize"<<endl;
    
    if(!WindowInitialized)
    {
    	LogFile<<"Game2: Window was not initialized"<<endl;
    	return 0;
	}
	
	//randomize
    LogFile<<"Game2: randomize rand by time"<<endl; 
    srand(time(NULL));
    
    //load sound system
    if(!SoundSystem.Initialize()) return false;
    SoundSystem.SetVolume(1.0f);
    if(!SoundSystem.CreateSound(&sndLevel, "sound/background.mp3")) return false;
    if(!SoundSystem.CreateSound(&sndPickup, "sound/pickup.mp3")) return false;
    if(!SoundSystem.CreateSound(&sndMenuSelect, "sound/menuselect.mp3")) return false;
    if(!SoundSystem.CreateSound(&sndMenuClick, "sound/menuclick.mp3")) return false;
    
    //prepare view
	Projection = glm::ortho(0.0f, (float)WindowWidth * CoordinateScale, 0.0f, (float)WindowHeight * CoordinateScale, -5.0f, 5.0f);
	CameraPosition = glm::vec3(0.0f, 0.0f, 1.0f);
	CameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	Model = glm::mat4(1.0f);
	View = glm::lookAt(CameraPosition, CameraDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    MVPdefault = MVP = Projection * View * Model;
    
    //prepare vertex array - for all programs
    glGenVertexArrays(1, &VertexArrayId);
	glBindVertexArray(VertexArrayId);
	GLenum Error = glGetError();
	if(Error != GL_NO_ERROR)
	{
		LogFile<<"Game2: "<<(char*)gluErrorString(Error)<<" "<<Error<<endl;
		return false;
	}
	
	LogFile<<"Game2: Load shaders"<<endl;
	ProgramId = LoadShaders((char*)"shaders/main2.vertexshader.glsl", (char*)"shaders/main2.fragmentshader.glsl");
	if(!ProgramId) return false;
	MVPId = glGetUniformLocation(ProgramId, "MVP");
	if(MVPId == -1)
	{
		LogFile<<"Game2: Can't get MVP uniform"<<endl;
		return false;
	}
	ColorId = glGetUniformLocation(ProgramId, "sameColor");
	if(ColorId == -1)
	{
		LogFile<<"Game2: Can't get Color uniform"<<endl;
		return false;
	}
	Sampler2DId = glGetUniformLocation(ProgramId, "mainSampler");
	if(Sampler2DId == -1)
	{
		LogFile<<"Game2: Can't get texture uniform"<<endl;
		return false;
	}
	UseTextureId = glGetUniformLocation(ProgramId, "useSampler");
	if(UseTextureId == -1)
	{
		LogFile<<"Game2: Can't get usetexture uniform"<<endl;
		return false;
	}
	UseOffsetVertexId = glGetUniformLocation(ProgramId, "useOffsetVertex");
	if(UseOffsetVertexId == -1)
	{
		LogFile<<"Game2: Can't get useoffset vertex uniform"<<endl;
		return false;
	}
	UseOffsetUVId = glGetUniformLocation(ProgramId, "useOffsetUV");
	if(UseOffsetUVId == -1)
	{
		LogFile<<"Game2: Can't get useoffset uv uniform"<<endl;
		return false;
	}
	
	//local, menu textures
	if(!(txLogo = TextureLoader.LoadTexture("textures/tex08.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;
	if(!(txWait = TextureLoader.LoadTexture("textures/tex17.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;
	if(!(txLose = TextureLoader.LoadTexture("textures/tex28.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;
	if(!(txFont = TextureLoader.LoadTexture("textures/tex01.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;
	if(!(txMenuPad = TextureLoader.LoadTexture("textures/tex03.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;

	//stage textures
	if(!(txBuffs = TextureLoader.LoadTexture("textures/tex31.png", 6, 1, 0, txBuffs_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;//23 buffs
	txStage.push_back(TextureLoader.LoadTexture("textures/tex30.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE));//24 items
	txStage.push_back(TextureLoader.LoadTexture("textures/tex19.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE));//18 hero
	txStage.push_back(TextureLoader.LoadTexture("textures/tex10.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_REPEAT));
	txStage.push_back(TextureLoader.LoadTexture("textures/tex32.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE));//20
	txStage.push_back(TextureLoader.LoadTexture("textures/tex11.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE));
	txStage.push_back(TextureLoader.LoadTexture("textures/tex06.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_REPEAT));//06//05
	txStage.push_back(TextureLoader.LoadTexture("textures/tex22.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_REPEAT));
	txStage.push_back(TextureLoader.LoadTexture("textures/tex21.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE));
	txStage.push_back(TextureLoader.LoadTexture("textures/tex29.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE));
	txStage.push_back(TextureLoader.LoadTexture("textures/tex01.png", 1, 1, 0, OneTexture_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE));
	
	//localbuffer
	try
	{
		LogoQuad = new MObject;
		WaitQuad = new MObject;
		LoseQuad = new MObject;
	}
	catch(bad_alloc& ba)
	{
		LogFile<<"Game2: Can't allocate memory for objects"<<endl;
		return false;
	}
	if(!LocalBuffer.Initialize(GL_STATIC_DRAW)) return false;
	if(!LocalBuffer.AddObject(LogoQuad, 2, 1, 4, 4, 0, 0, 1, 1, txLogo->Id)) return false;
	if(!LocalBuffer.AddObject(WaitQuad, 2, 1, 4, 4, 0, 0, 1, 1, txWait->Id)) return false;
	if(!LocalBuffer.AddObject(LoseQuad, 2, 1, 4, 4, 0, 0, 1, 1, txLose->Id)) return false;
	if(!LocalBuffer.DisposeAll()) return false;
	
	//stage
	if(!Stage.Initialize(&txStage, txBuffs, txBuffs_cnt, m_hWnd, &LiquidColor[0])) return false;
	Stage.Size((float)WindowWidth * CoordinateScale, (float)WindowHeight * CoordinateScale);
	
	//init text
	if(!TextController.Initialize(WindowWidth * CoordinateScale, WindowHeight * CoordinateScale, txFont->Id)) return false;
	//menus
	if(!MenusInitialize(&WindowWidth, &WindowHeight, &CoordinateScale, txFont->Id, txMenuPad->Id, &Resolutions)) return false;
	
	//set colors
	SetColorLimits(1, 0, 0.005);
	SetCurrentColor(1, 1, 1, 1);
	SetColor(HeroColor, 1, 1, 1, 1);
	SetColor(LiquidColor, 1.0f, 1.0f, 1.0f, 1.0f);
	
	//last preparations
	Stop();
	SetCurrentColor(0, 0, 0, 1);
	pDrawFunc = &MGame2::DrawLogo;
    
    return true;
}

void MGame2::Start()
{
    Pause = false;
    Stage.Start();
}

void MGame2::Stop()
{
    Pause = true;
    Stage.Stop();
}

void MGame2::SetColorLimits(float Max, float Min, float Step)
{
	ColorLimit[0] = Max;
	ColorLimit[1] = Min;
	ColorLimit[2] = Step;
}

void MGame2::SetCurrentColor(float R, float G, float B, float A)
{
	CurrentColor[3] = CurrentColor[2] = CurrentColor[1] = CurrentColor[0] = glm::vec4(R, G, B, A);
}

void MGame2::SetColor(glm::vec4* pColor4, float R, float G, float B, float A)
{
	if(!pColor4) return;
	pColor4[3] = pColor4[2] = pColor4[1] = pColor4[0] = glm::vec4(R, G, B, A);
}

void MGame2::DrawGame()
{
	if(!Pause)
	{
		Stage.WorldStep();
		if(Stage.IsItemCollected())
			if(SoundEnabled) SoundSystem.PlaySound(sndPickup, &chnOther);
		CameraPosition.y = CameraDirection.y = ViewOffset + round((Stage.GetHeroCenter().y - Stage.GetHeroStartCenter().y) * CameraRound) / CameraRound;
		View = glm::lookAt(CameraPosition, CameraDirection, glm::vec3(0, 1, 0));
		if(ViewScale != 1.0f) View = glm::scale(View, glm::vec3(ViewScale, ViewScale, 0.0f));
    	MVP = Projection * View * Model;
    	
    	switch(Stage.GetState())
		{
			case STT_WIN:
				if(SoundEnabled) SoundSystem.StopPlay(chnLevel);
				Stop();
				SetColorLimits(1, 0, -0.01);
				SetCurrentColor(0.9f, 0.9f, 0.9f, 1.0f);
				break;
			case STT_LOSE:
				if(SoundEnabled) SoundSystem.StopPlay(chnLevel);
				Stop();
				SetColorLimits(1, 0, -0.01);
				SetCurrentColor(0.9f, 0.9f, 0.9f, 1.0f);
				break;
		}
	}
	else
	{
		if(Stage.GetState() == STT_WIN || Stage.GetState() == STT_LOSE)
		{
			SetColor(CurrentColor, CurrentColor[0][0] + ColorLimit[2], CurrentColor[0][1] + ColorLimit[2], CurrentColor[0][2] + ColorLimit[2], CurrentColor[0][3]);
			SetColor(LiquidColor, LiquidColor[0][0] + ColorLimit[2], LiquidColor[0][1] + ColorLimit[2], LiquidColor[0][2] + ColorLimit[2], LiquidColor[0][3]);
			if(CurrentColor[0][0] <= ColorLimit[1])
			{
				if(Stage.GetState() == STT_WIN) pDrawFunc = &MGame2::DrawWait;
				else pDrawFunc = &MGame2::DrawLose;
				SetCurrentColor(1.0f, 1.0f, 1.0f, 1.0f);
				return;
			}
		}
	}
	
	//prepare shader, matrix, color, uniforms
	glUseProgram(ProgramId);
	glUniformMatrix4fv(MVPId, 1, GL_FALSE, &MVP[0][0]);
	
	glUniform4fv(ColorId, 1, &CurrentColor[0][0]);//set white color
	glUniform1i(UseOffsetVertexId, 0);//disable vertex offset
	glUniform1i(UseOffsetUVId, 0);//disable uv offset
	glUniform1i(UseTextureId, 1);//enable texture
	
	glEnable(GL_BLEND);
	
	glEnableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glEnableVertexAttribArray(SHR_LAYOUT_UV);
	Stage.DrawBackground();
	
	glUniform1i(UseOffsetVertexId, 1);
	glUniform1i(UseOffsetUVId, 1);
	glEnableVertexAttribArray(SHR_LAYOUT_OFFSET_VERTEX);
	glEnableVertexAttribArray(SHR_LAYOUT_OFFSET_UV);
	Stage.DrawLevel();
	glDisableVertexAttribArray(SHR_LAYOUT_OFFSET_VERTEX);
	glDisableVertexAttribArray(SHR_LAYOUT_OFFSET_UV);
	glUniform1i(UseOffsetVertexId, 0);
	glUniform1i(UseOffsetUVId, 0);
	
	Stage.DrawOther();
	
	glUniform4fv(ColorId, 1, &LiquidColor[0][0]);//while level end - liquid is not hiding?
	Stage.DrawFlow();
	
	glUniform4fv(ColorId, 1, &CurrentColor[0][0]);
	if(Stage.GetFogEnabled())
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_ALPHA_TEST);
		Stage.DrawNoFog();
		glUniformMatrix4fv(MVPId, 1, GL_FALSE, &MVPdefault[0][0]);
		Stage.DrawFog();
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_DEPTH_TEST);
	}
	
	glUniformMatrix4fv(MVPId, 1, GL_FALSE, &MVPdefault[0][0]);
	Stage.DrawInterface();
	
	glDisableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glDisableVertexAttribArray(SHR_LAYOUT_UV);
	
	glDisable(GL_BLEND);
}

void MGame2::DrawWait()
{
	/*
	if(BlinkEnable)
	{
		//don't use 3d color - it's alpha
		SetCurrentColor(CurrentColor[0][0] + ColorLimit[2], CurrentColor[0][1] + ColorLimit[2], CurrentColor[0][2] + ColorLimit[2], CurrentColor[0][3]);
		//if we reached some of limits change add value
		if(CurrentColor[0][0] <= ColorLimit[1] || CurrentColor[0][0] >= ColorLimit[0]) ColorLimit[2] = -ColorLimit[2];
	}
	*/
	glUseProgram(ProgramId);
	glUniformMatrix4fv(MVPId, 1, GL_FALSE, &MVPdefault[0][0]);
	glUniform4fv(ColorId, 1, &CurrentColor[0][0]);//set white color
	glUniform1i(UseOffsetVertexId, 0);//disable vertex offset
	glUniform1i(UseOffsetUVId, 0);//disable uv offset
	glUniform1i(UseTextureId, 1);//enable texture
	
	glEnableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glEnableVertexAttribArray(SHR_LAYOUT_UV);
	LocalBuffer.DrawObject(WaitQuad);
	TextController.DrawHint();
	TextController.DrawGameString(1);
	glDisableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glDisableVertexAttribArray(SHR_LAYOUT_UV);
}

void MGame2::DrawLogo()
{
	SetCurrentColor(CurrentColor[0][0] + ColorLimit[2], CurrentColor[0][1] + ColorLimit[2], CurrentColor[0][2] + ColorLimit[2], CurrentColor[0][3]);
	if(CurrentColor[0][0] >= ColorLimit[0]) ColorLimit[2] = -ColorLimit[2];
	if(CurrentColor[0][0] < ColorLimit[1]) 
	{
		pDrawFunc = &MGame2::DrawMenu;
		SetCurrentColor(1, 1, 1, 1);
		return;
	}
	
	glUseProgram(ProgramId);
	glUniformMatrix4fv(MVPId, 1, GL_FALSE, &MVPdefault[0][0]);
	glUniform4fv(ColorId, 1, &CurrentColor[0][0]);
	glUniform1i(UseOffsetVertexId, 0);
	glUniform1i(UseOffsetUVId, 0);
	glUniform1i(UseTextureId, 1);
	
	glEnableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glEnableVertexAttribArray(SHR_LAYOUT_UV);
	LocalBuffer.DrawObject(LogoQuad);
	glDisableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glDisableVertexAttribArray(SHR_LAYOUT_UV);
}

void MGame2::DrawMenu()
{
	glUseProgram(ProgramId);
	glUniformMatrix4fv(MVPId, 1, GL_FALSE, &MVPdefault[0][0]);
	glUniform1i(UseOffsetVertexId, 0);//disable vertex offset
	glUniform1i(UseOffsetUVId, 0);//disable uv offset;
	glUniform4fv(ColorId, 1, &CurrentColor[0][0]);
	
	glEnable(GL_BLEND);
	glUniform1i(UseTextureId, 1);//enable texture
	glEnableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glEnableVertexAttribArray(SHR_LAYOUT_UV);
	pCurrentMenu->Draw();
	glDisableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glDisableVertexAttribArray(SHR_LAYOUT_UV);
	glDisable(GL_BLEND);
}

void MGame2::DrawLose()
{
	glUseProgram(ProgramId);
	glUniformMatrix4fv(MVPId, 1, GL_FALSE, &MVPdefault[0][0]);
	glUniform4fv(ColorId, 1, &CurrentColor[0][0]);
	glUniform1i(UseOffsetVertexId, 0);
	glUniform1i(UseOffsetUVId, 0);
	glUniform1i(UseTextureId, 1);
	
	glEnableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glEnableVertexAttribArray(SHR_LAYOUT_UV);
	LocalBuffer.DrawObject(LoseQuad);
	TextController.DrawGameString(2);
	glDisableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glDisableVertexAttribArray(SHR_LAYOUT_UV);
}

void MGame2::DrawThanks()
{
	glUseProgram(ProgramId);
	glUniformMatrix4fv(MVPId, 1, GL_FALSE, &MVPdefault[0][0]);
	glUniform4fv(ColorId, 1, &CurrentColor[0][0]);
	glUniform1i(UseOffsetVertexId, 0);
	glUniform1i(UseOffsetUVId, 0);
	glUniform1i(UseTextureId, 1);
	
	glEnableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glEnableVertexAttribArray(SHR_LAYOUT_UV);
	TextController.DrawThanks();
	glDisableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glDisableVertexAttribArray(SHR_LAYOUT_UV);
}

void MGame2::OnDraw()
{
	if(pDrawFunc) ((*this).*(pDrawFunc))();
}

void MGame2::OnActivate(WPARAM wParam)
{
	switch(LOWORD(wParam))
	{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			if(pDrawFunc == &MGame2::DrawGame) Start();
			LogFile<<"Game2: window activated!"<<endl;
			break;
		case WA_INACTIVE:
			if(pDrawFunc == &MGame2::DrawGame) Stop();
			LogFile<<"Game2: window deactivated!"<<endl;
			break;
	}
}

void MGame2::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	Key[wParam] = 1;

	if(pDrawFunc == &MGame2::DrawGame)
	{
		if(Key[VK_ESCAPE])
		{
			SendMessage(m_hWnd, WM_DESTROY, 0, 0);
			return;
		}
	}
	if(pDrawFunc == &MGame2::DrawWait)
	{
		if(Key[VK_ESCAPE])
		{
			pDrawFunc = &MGame2::DrawMenu;
			return;
		}
		if(Key[VK_SPACE]) 
		{
			if(!Stage.Next())
			{
				MessageBox(NULL, "Sorry. Some error(s) ocurred. See log for details", "Error", 0);
				SendMessage(m_hWnd, WM_DESTROY, 0, 0);
			}
			Sleep(1);
			pDrawFunc = &MGame2::DrawGame;
			Start();
			if(SoundEnabled) SoundSystem.PlaySound(sndLevel, &chnLevel, true);
			return;
		}
	}
	if(pDrawFunc == &MGame2::DrawLose)
	{
		if(Key[VK_SPACE])
		{
			if(!Stage.Next())
			{
				MessageBox(NULL, "Sorry. Some error(s) ocurred. See log for details", "Error", 0);
				SendMessage(m_hWnd, WM_DESTROY, 0, 0);
			}
			Sleep(1);
			pDrawFunc = &MGame2::DrawGame;
			Start();
			if(SoundEnabled) SoundSystem.PlaySound(sndLevel, &chnLevel, true);
			return;
		}
		if(Key[VK_ESCAPE]) 
		{
			pDrawFunc = &MGame2::DrawMenu;
			return;
		}
	}
	if(pDrawFunc == &MGame2::DrawThanks)
	{
		if(Key[VK_ESCAPE]) 
		{
			pDrawFunc = &MGame2::DrawMenu;
			return;
		}
	}
	if(pDrawFunc == &MGame2::DrawMenu)
	{
		int Ret = MenuKeyChange(wParam, lParam);
		if(Ret >= 0) 
		{
			CustomKeys[Ret] = wParam;
			return;
		}
		if(Key[VK_ESCAPE])
		{
			SendMessage(m_hWnd, WM_DESTROY, 0, 0);
			return;
		}
		if(Key[VK_RETURN])
		{
			MenusProcess();
			if(SoundEnabled) SoundSystem.PlaySound(sndMenuClick, &chnOther);
			return;
		}
		if(Key[VK_UP])
		{
			pCurrentMenu->SelectPrevPad();
			if(SoundEnabled) SoundSystem.PlaySound(sndMenuSelect, &chnOther);
			return;
		}
		if(Key[VK_DOWN])
		{
			pCurrentMenu->SelectNextPad();
			if(SoundEnabled) SoundSystem.PlaySound(sndMenuSelect, &chnOther);
			return;
		}
	}
}

void MGame2::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	Key[wParam] = 0;
	if(pDrawFunc == &MGame2::DrawGame)
	{
	}
}

void MGame2::OnClose()
{
	Stop();
	LogFile<<"Game2: Stopped."<<endl;
	
	SoundSystem.StopPlay(chnLevel);
	SoundSystem.StopPlay(chnOther);
	SoundSystem.ReleaseSound(sndLevel);
	SoundSystem.ReleaseSound(sndPickup);
	SoundSystem.ReleaseSound(sndMenuSelect);
	SoundSystem.ReleaseSound(sndMenuClick);
	SoundSystem.Close();
	LogFile<<"Game2: Sound system close."<<endl;
	
	LocalBuffer.Close();
	if(LogoQuad) delete LogoQuad;
	if(WaitQuad) delete WaitQuad;
	if(LoseQuad) delete LoseQuad;
	LogFile<<"Game2: Local buffer free"<<endl;
	
	TextController.Close();
	LogFile<<"Game: Text controller free"<<endl;
	
	MenusClose();
	LogFile<<"Game2: Menus free"<<endl;
	
	Stage.Close();
	TextureLoader.DeleteTexture(txStage[0], OneTexture_cnt);
	TextureLoader.DeleteTexture(txStage[1], OneTexture_cnt);
	TextureLoader.DeleteTexture(txStage[2], OneTexture_cnt);
	TextureLoader.DeleteTexture(txStage[3], OneTexture_cnt);
	TextureLoader.DeleteTexture(txStage[4], OneTexture_cnt);
	TextureLoader.DeleteTexture(txStage[5], OneTexture_cnt);
	TextureLoader.DeleteTexture(txStage[6], OneTexture_cnt);
	TextureLoader.DeleteTexture(txStage[7], OneTexture_cnt);
	TextureLoader.DeleteTexture(txStage[8], OneTexture_cnt);
	TextureLoader.DeleteTexture(txStage[9], OneTexture_cnt);
	txStage.clear();
	LogFile<<"Game2: Stage free"<<endl;
	
	TextureLoader.DeleteTexture(txLogo, OneTexture_cnt);
	TextureLoader.DeleteTexture(txLose, OneTexture_cnt);
	TextureLoader.DeleteTexture(txWait, OneTexture_cnt);
	TextureLoader.DeleteTexture(txFont, OneTexture_cnt);
	TextureLoader.DeleteTexture(txMenuPad, OneTexture_cnt);
	TextureLoader.DeleteTexture(txBuffs, txBuffs_cnt);
	TextureLoader.Close();
	
	glDeleteProgram(ProgramId);
	glDeleteVertexArrays(1, &VertexArrayId);
	LogFile<<"Game2: Shaders free"<<endl;
	
	if(Key) delete [] Key;
	LogFile<<"Game2: Keys free"<<endl;
}

void MGame2::OnSize()
{
	if(pDrawFunc == NULL) return;
	cout<<"Size function!"<<endl;
	
	SetViewScale(ViewScale);
	
	//change projection matix and default MVP
	Projection = glm::ortho(0.0f, (float)WindowWidth * CoordinateScale, 0.0f, (float)WindowHeight * CoordinateScale, -5.0f, 5.0f);
    MVPdefault = Model * Projection * glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    //size all text variables
    TextController.Size(WindowWidth * CoordinateScale, WindowHeight * CoordinateScale);
    
    //update wait and logo object
    if(WaitQuad && LogoQuad && LoseQuad)
	{
		stQuad Vertex;
		SetQuad(Vertex, HalfWindowSize[0] * CoordinateScale - 2, HalfWindowSize[1] * CoordinateScale - 2, 4, 4);
		WaitQuad->SetVertex(Vertex);
		if(!LocalBuffer.UpdateObject(WaitQuad)) LogFile<<"Game2: Cant update wait object"<<endl;
		LogoQuad->SetVertex(Vertex);
		if(!LocalBuffer.UpdateObject(LogoQuad)) LogFile<<"Game2: Cant update logo object"<<endl;
		LoseQuad->SetVertex(Vertex);
		if(!LocalBuffer.UpdateObject(LoseQuad)) LogFile<<"Game2: Cant update lose object"<<endl;
	}
    
    //size stage and interface
    Stage.Size((float)WindowWidth * CoordinateScale, (float)WindowHeight * CoordinateScale);
    
	//update menus
	MenusResize();
	
	//center view using translate matrix
	CameraPosition = glm::vec3(Stage.GetLevelCenter().x - ViewCenter.x, 0.0f, 1.0f);
	CameraDirection = glm::vec3(Stage.GetLevelCenter().x - ViewCenter.x, 0.0f, 0.0f);
	
	//change viewport
	glViewport(0, 0, WindowWidth, WindowHeight);//IMPORTANT!
}

void MGame2::SetViewScale(float inViewScale)
{
	ViewScale = inViewScale;
	ViewCenter = glm::vec2((HalfWindowSize[0] + WindowWidth * (1.0f - ViewScale) * 0.5f) * CoordinateScale, (HalfWindowSize[1] + WindowHeight * (1.0f - ViewScale) * 0.5f) * CoordinateScale);
}

void MGame2::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	if(pDrawFunc == &MGame2::DrawMenu) pCurrentMenu->SelectPadByMouse(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
}

void MGame2::OnMouseLButton(WPARAM wParam, LPARAM lParam)
{
	if(pDrawFunc == &MGame2::DrawMenu) MenusProcess();
}

void MGame2::OnMenuStart()
{
	pDrawFunc = &MGame2::DrawWait;
}

void MGame2::OnMenuThanks()
{
	pDrawFunc = &MGame2::DrawThanks;
}

void MGame2::OnMenuExit()
{
	SendMessage(m_hWnd, WM_DESTROY, 0, 0);
}

void MGame2::OnMenuGraphicsAccept()
{
	//need extend check (if window settings not changed - don't touch them)
	if(pCurrentMenu->GetRotatePadCurrentString(1))
	{
		CurrentResolution = Resolutions[pCurrentMenu->GetRotatePadCurrentString(0)];
		EnterFullscreen();
	}
	else
	{
		if(!Fullscreen) ChangeWindowSize(Resolutions[pCurrentMenu->GetRotatePadCurrentString(0)].X, Resolutions[pCurrentMenu->GetRotatePadCurrentString(0)].Y);
		else
		{
			CurrentResolution = InitialResolution;
			ExitFullscreen(Resolutions[pCurrentMenu->GetRotatePadCurrentString(0)].X, Resolutions[pCurrentMenu->GetRotatePadCurrentString(0)].Y);
		}
	}
}

void MGame2::OnMenuGameplayAccept()
{
	Stage.SetFogEnabled(pCurrentMenu->GetRotatePadCurrentString(0));
	//need action for weather
	//need action for softcore
}

void MGame2::OnMenuSoundAccept()
{
	SoundEnabled = pCurrentMenu->GetRotatePadCurrentString(0);
	SoundSystem.SetVolume(1 - pCurrentMenu->GetRotatePadCurrentString(1) * 0.1f);
}
