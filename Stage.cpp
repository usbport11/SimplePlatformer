#include "stdafx.h"
#include "Stage.h"

map<UINT_PTR, MStage*> FrameTimerMap;

void CALLBACK MStage::FrameTimerFunction(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	MStage* pStage = FrameTimerMap[idEvent];
	if(idEvent == pStage->FrameTimer.Id)
	{
		//increase frame
		for(unsigned int i=0; i<pStage->Items.size(); i++)
		{
			pStage->Items[i]->IncreaseAnimationFrame();
			pStage->ObjectBuffer->UpdateObject(pStage->Items[i]);
		}
		pStage->ExitObject->IncreaseAnimationFrame();
		pStage->ObjectBuffer->UpdateObject(pStage->ExitObject);
		pStage->Hero->IncreaseAnimationFrame();
		pStage->ObjectBuffer->UpdateObject(pStage->Hero);
		return;
	}
}

MStage::MStage()
{
	pTextures = NULL;
	Level2 = NULL;
	ExitObject = NULL;
	Hero = NULL;
	FlowObject = NULL;
	ObjectBuffer = NULL;
	Interface = NULL;
	Background = NULL;
	Fog = NULL;
	NoFog = NULL;
	
	HeroCenter = glm::vec2(0.0f, 0.0f);
	FogSpeed = glm::vec2(-0.002f, 0.0f);
	FogEnabled = false;
	
	MovableObjectSize = glm::vec2(0.32f, 0.32f);
	ItemSize = glm::vec2(0.16f, 0.16f);
	LevelUnitSize = glm::vec2(0.32f, 0.32f);
	
	LevelTypesCount = 3;
}

MStage::~MStage()
{
	pTextures = NULL;
	Level2 = NULL;
	ExitObject = NULL;
	Hero = NULL;
	FlowObject = NULL;
	ObjectBuffer = NULL;
	Background = NULL;
}

void MStage::Start()
{
	FrameTimer.Start();
	FlowObject->StartSizeTimer();
}

void MStage::Stop()
{
	FrameTimer.Stop();
	FlowObject->StopSizeTimer();
}

void MStage::SetState(unsigned char inState)
{
	State = inState;
}

bool MStage::PlaceItems(unsigned int ItemsCount)
{
	if(!ItemsCount)
	{
		LogFile<<"ItemController: NULL Items count"<<endl;
		return false;
	}

	bool* UsedBoxes = new bool [Level2->GetBoxesCount()];
	memset(UsedBoxes, 0, Level2->GetBoxesCount());
	UsedBoxes[0] = true; //disable placing items on first box
	UsedBoxes[Level2->GetBoxesCount() - 1] = true; //disable placing items on last box
	stQuad BoxQuad;
	unsigned int RandItemType, BoxNumber;
	for(int i=0; i<ItemsCount; i++)
	{
		BoxNumber = Level2->GetRandomBoxQuad(BoxQuad);
		if(UsedBoxes[BoxNumber])
		{
			cout<<"Relocate item"<<endl;
			i --;
			continue;
		}
		UsedBoxes[BoxNumber] = true;
		RandItemType = (rand() % (int)ITEM_TYPES_COUNT);// + 1;
		Items.push_back(new MItem(RandItemType));
		Items[i]->AddParameter(5 - rand() % 2);//from 2 to 5 seconds
		if(!ObjectBuffer->AddObject(Items[i], GetQuadCenter(BoxQuad).x - 0.08f, BoxQuad.p[1].y + 0.08f, ItemSize.x, ItemSize.y, 0, 0, 1, 1, pTextures->at(TX_ITEMS)->Id)) return false;
		if(!Items[i]->SetAnimations(8, 4)) return false;//8 - because 1/8 better than 1/6
		if(!Items[i]->SetAnimationType(RandItemType, ANMT_LOOP)) return false;//-1
		if(!Items[i]->SetCurrentAnimation(RandItemType)) return false;//-1
		Items[i]->StartAnimation();
		if(!Items[i]->AddPhysics(World, b2_kinematicBody, true, false, OT_COLLECTABLE, OT_MOVABLE)) return false;//true,true
	}
	if(UsedBoxes) delete [] UsedBoxes;
	
	return true;
}

void MStage::CheckItems()
{
	ItemCollected = false;
	for(int i=0; i<Items.size(); i++)
	{
		if(Items[i]->GetCollected()) 
		{
			ItemCollected = true;
			RemoveItem(i);
		}
	}
}

bool MStage::RemoveItem(unsigned int Number)
{
	cout<<"Item number: "<<Number<<" Item type: "<<Items[Number]->GetType()<<endl;
	//remove physic
	Items[Number]->PhysicEnable(false);
	//migrate to collected items vector and remove from base
	CollectedItems.push_back(Items[Number]);
	Items.erase(Items.begin() + Number);
}

unsigned char MStage::GetState()
{
	return State;
}

bool MStage::Initialize(vector<stTexture*>* inpTextures, stTexture* pInterfaceBuffs, unsigned int InterfaceBuffsCount, HWND hWnd, glm::vec4* inpLiquidColor)
{
	SetState(STT_INIT);

	if(!inpLiquidColor)
	{
		LogFile<<"Stage: Wrong liquid color ponter"<<endl;
		return false;
	}
	pLiquidColor = inpLiquidColor;

	if(!inpTextures || inpTextures->size() <= TX_EXIT)
	{
		LogFile<<"Stage: Wrong texture params"<<endl;
		return false;
	}
	
	pTextures = inpTextures;
	for(unsigned int i=0; i<=TX_EXIT; i++)
	{
		if(!pTextures->at(i))
		{
			LogFile<<"Stage: Some of textures is NULL"<<endl;
			return false;
		}
	}
	
	//timer
	if(!FrameTimer.Set(hWnd, 1001, 250, FrameTimerFunction)) return false;
	FrameTimerMap[FrameTimer.Id] = this;
	FrameTimer.Enabled = false;
	
	//level number
	LevelNumber = 0;
	
	//box2d world values
	Gravity = b2Vec2(0.0, -20.0);
	timeStep = 1.0f / 60.0f;
	velocityIterations = 6;
	positionIterations = 2;
	World = new b2World(Gravity);
    World->SetContactListener(&OCL);
	
	//allocate objects in memory
	try
	{
		Level2 = new MLevel2;
		ExitObject = new MActivatableObject(AOT_EXIT);
		Hero = new MHero;
		FlowObject = new MFlowObject;
		ObjectBuffer = new MObjectBuffer;
		Interface = new MInterface;
		Background = new MBackground3;
		Fog = new MObject;
		NoFog = new MObject;
	}
	catch(bad_alloc& ba)
	{
		LogFile<<"Stage: Can't allocate memory for objects"<<endl;
		return false;
	}
	
	//level
	LevelSize = glm::vec2(LevelUnitSize.x * 25, LevelUnitSize.y * 18);
    Level2->SetUnit(LevelUnitSize.x, LevelUnitSize.y, 25, 18);
    Level2->SetBoxLimits(3, 4, 1, 2);
    Level2->SetHoleLimit(2, 2);
    if(!Level2->SetTextureData(pTextures->at(TX_LEVEL)->Id, 320, 96, 10, LevelTypesCount)) return false;
	if(!Level2->SetEdgesOffsets(0, 2)) return false;
    if(!Level2->Initialize(World)) return false;
    
    //background
	if(!Background->Initialize(pTextures->at(TX_BACKGROUND3), 12, 160, 384, 100, 0.0f, -0.64f)) return false;
    
    //interface
    if(!Interface->Initialize(pInterfaceBuffs, InterfaceBuffsCount, pTextures->at(TX_UIBOTTOM), &LevelSize)) return false;
    if(!Interface->InitializeStats(pTextures->at(TX_FONT)->Id, 0.28f, 0.20f, 8.0f, 6.0f)) return false;
	
	//object buffer init
	if(!ObjectBuffer->Initialize(GL_STREAM_DRAW)) return false;
	//fill buffer start data (vertex and uv does not matter)
	if(!ObjectBuffer->AddObject(Hero, 0, 0, MovableObjectSize.x, MovableObjectSize.y, 0, 0, 1, 1, pTextures->at(TX_HERO)->Id)) return false;//important to give right initial size
		if(!Hero->SetAnimations(4, 4)) return false;
    	if(!Hero->SetAnimationType(ANM_DEAD, ANMT_ONEWAY)) return false;
		if(!Hero->SetAnimationType(ANM_JUMP, ANMT_ONEWAY)) return false;
		if(!Hero->SetAnimationType(ANM_MOVE, ANMT_LOOP)) return false;
		if(!Hero->SetAnimationType(ANM_STAY, ANMT_LOOP)) return false;
		if(!Hero->SetCurrentAnimation(ANM_STAY)) return false;
		Hero->SetAnimationControl(true);
		Hero->StartAnimation();
		if(!Hero->AddPhysics(World, b2_dynamicBody, false, false, OT_MOVABLE, OT_BOUNDARY | OT_COLLECTABLE | OT_ACTIVATABLE, b2Vec2(0.02f, 0.02f))) return false;//false, true
		if(!Hero->SetMoveParameters(0.05f, 0.6f, 2.0f)) return false;//0.05,0.9,2.0
		if(!Hero->SetMoveKeys(VK_LEFT, VK_RIGHT, VK_UP)) return false;
		if(!Hero->SetInterface(Interface)) return false;
		if(!Hero->InitTimers(ITEM_TYPES_COUNT)) return false;
		
	if(!ObjectBuffer->AddObject(ExitObject, 0, 0, MovableObjectSize.x, MovableObjectSize.y, 0, 0, 1, 1, pTextures->at(TX_EXIT)->Id)) return false;
		if(!ExitObject->SetAnimations(1, 4)) return false;
		if(!ExitObject->SetAnimationType(0, ANMT_LOOP)) return false;
		if(!ExitObject->SetCurrentAnimation(0)) return false;
		ExitObject->StartAnimation();
		if(!ExitObject->AddPhysics(World, b2_kinematicBody, true, true, OT_ACTIVATABLE, OT_MOVABLE)) return false;
		
	if(!ObjectBuffer->AddObject(FlowObject, 0, 0, 1, 1, 0, 0, 1, 1, pTextures->at(TX_FLOW)->Id)) return false;
		if(!FlowObject->SetSizeTimer(hWnd, 2000, false)) return false;
	
	//test	
	if(!ObjectBuffer->AddObject(Fog, 0, 0, 8, 6, 0, 0, 1, 1, pTextures->at(TX_FOG)->Id)) return false;
	if(!ObjectBuffer->AddObject(NoFog, 0, 0, 2, 2, 0, 0, 1, 1, pTextures->at(TX_NOFOG)->Id)) return false;
	ObjectBuffer->DisposeAll();
	
	return true;
}

bool MStage::Next()
{
	SetState(STT_PREPARE);
	
	stQuad BoxQuad;
	
	//clearing
	Level2->Clear();
	Interface->Clear();
	ClearItems();
	Hero->ClearActiveTimers();
	
	//liquid color
	if(!pLiquidColor)
	{
		LogFile<<"Stage: Wrong liquid color ponter"<<endl;
		return false;
	}
	
	int RandValue = rand() % LevelTypesCount;
	switch(RandValue)//wrong number sequence (need change background code (1-UV.y))
	{
		case 2://lava
			pLiquidColor[0] = pLiquidColor[1] = pLiquidColor[2] = pLiquidColor[3] = glm::vec4(1.5f, 0.0f, 0.0f, 1.0f);
			break;
		case 1://frozen
			pLiquidColor[0] = pLiquidColor[1] = pLiquidColor[2] = pLiquidColor[3] = glm::vec4(0.0f, 0.0f, 1.5f, 1.0f);
			break;
		case 0://sand
			pLiquidColor[0] = pLiquidColor[1] = pLiquidColor[2] = pLiquidColor[3] = glm::vec4(1.5, 1.5f, 0.0f, 1.0f);
			break;
	}
	
	//level
	LevelNumber ++;
	LevelProgress = 0;
	Level2->SetUnit(LevelUnitSize.x, LevelUnitSize.y, 25, 18 + LevelNumber * 4);
	LevelSize = glm::vec2(LevelSize.x, LevelUnitSize.y * (18 + LevelNumber * 4));
	if(!Level2->Create(RandValue)) return false;
	cout<<"Level: "<<LevelNumber<<endl;
	
	//background
	//if(!Background->Create(18 + LevelNumber * 4 + 4, 8, 6, 0.32f)) return false;
	if(!Background->SetLines(18 + LevelNumber * 4 + 4, 8, 6, 0.32f, LevelTypesCount)) return false;
	if(!Background->CreateByPartNumber(RandValue)) return false;
	
	//update interface stats
	if(!Interface->SetStatValue(0, LevelNumber)) return false;
	
	//items
	if(!PlaceItems(ITEM_TYPES_COUNT)) return false;
	
	//recreate exit object
	if(!ObjectBuffer->RemoveObject(ExitObject)) return false;
	ExitObject->PhysicEnable(false);
	BoxQuad = Level2->GetLastBoxQuad();
	if(!ObjectBuffer->AddObject(ExitObject, GetQuadCenter(BoxQuad).x - MovableObjectSize.x * 0.5f, BoxQuad.p[1].y + 0.01, MovableObjectSize.x, MovableObjectSize.y, 0, 0, 1, 1, pTextures->at(TX_EXIT)->Id)) return false;
		if(!ExitObject->SetAnimations(1, 4)) return false;
		if(!ExitObject->SetAnimationType(0, ANMT_LOOP)) return false;
		if(!ExitObject->SetCurrentAnimation(0)) return false;
		ExitObject->StartAnimation();
		if(!ExitObject->UpdatePhysics()) return false;
		ExitObject->PhysicEnable(true);
		ExitCenter = GetQuadCenter(ExitObject->GetVertex());
	
	//movable object (need think about remove object from buffer)
	if(!ObjectBuffer->RemoveObject(Hero)) return false;
	Hero->PhysicEnable(false);
	BoxQuad = Level2->GetFirstBoxQuad();
	if(!ObjectBuffer->AddObject(Hero, BoxQuad.p[1].x + MovableObjectSize.x * 0.5f, BoxQuad.p[1].y + 0.01, MovableObjectSize.x, MovableObjectSize.y, 0, 0, 1, 1, pTextures->at(TX_HERO)->Id)) return false;
		if(!Hero->SetAnimations(4, 4)) return false;
    	if(!Hero->SetAnimationType(ANM_DEAD, ANMT_ONEWAY)) return false;
		if(!Hero->SetAnimationType(ANM_JUMP, ANMT_ONEWAY)) return false;
		if(!Hero->SetAnimationType(ANM_MOVE, ANMT_LOOP)) return false;
		if(!Hero->SetAnimationType(ANM_STAY, ANMT_LOOP)) return false;
		if(!Hero->SetCurrentAnimation(ANM_STAY)) return false;
		Hero->StartAnimation();
		if(!Hero->UpdatePhysics()) return false;
		if(!Hero->SetMoveParameters(0.05f, 0.6f, 2.0f)) return false;//0.05,0.9,2.0
		Hero->SetMoveEnabled(true);
		Hero->SetJumpEnabled(true);
		Hero->PhysicEnable(true);
		HeroStartCenter = GetQuadCenter(Hero->GetVertex());
		HeroCenter = HeroStartCenter;
	
	//update flow object
	SetQuad(BoxQuad, 0, -0.64, 8, 0.32);
	if(!FlowObject->SetVertex(BoxQuad)) return false;
	SetQuad(BoxQuad, 0, 0, 1.5625, 0.0625);
	if(!FlowObject->SetUV(BoxQuad)) return false;
	FlowObject->Clear();
	if(!FlowObject->SetTextureSize(5.12f, 5.12f)) return false;
    if(!FlowObject->SetFlowVelocity(0, -0.0025)) return false;
    FlowObject->SetSizeVelocity(0, 0.16);
    FlowObject->SetSizeLimit(0, (18 + LevelNumber * 4 + 4) * LevelUnitSize.y);
	
	//update draw buffer
	if(!ObjectBuffer->DisposeAll()) return false;
	
	SetState(STT_READY);
	
	return true;
}

void MStage::ClearItems()
{
	for(unsigned int i=0; i<Items.size(); i++)
	{
		ObjectBuffer->RemoveObject(Items[i]);
		Items[i]->RemovePhysics();
		delete Items[i];
	}
	for(unsigned int i=0; i<CollectedItems.size(); i++)
	{
		ObjectBuffer->RemoveObject(CollectedItems[i]);
		delete CollectedItems[i];
	}
	Items.clear();
	CollectedItems.clear();
}

bool MStage::CanStartNextLevel()
{
	return ExitObject->GetCollided();
}

void MStage::WorldStep()
{
	Hero->Move();
	World->Step(timeStep, velocityIterations, positionIterations);
	FlowObject->Flow();
	ObjectBuffer->UpdateObject(Hero);
	ObjectBuffer->UpdateObject(FlowObject);
	CheckItems();
	Hero->TimeCheck();
	
	HeroCenter = GetQuadCenter(Hero->GetVertex());
	if(FogEnabled)
	{
		SetQuad(FogQuad, HeroCenter.x - 1.0f, HeroCenter.y - 1.0f, 2.0f, 2.0f);
		NoFog->SetVertex(FogQuad);
		ObjectBuffer->UpdateObject(NoFog);
		FogQuad = Fog->GetUV();
		MoveQuad(FogQuad, FogSpeed.x, FogSpeed.y);
		if(FogQuad.p[0].x >= 1) SetQuad(FogQuad, 0,0,1,1);
		Fog->SetUV(FogQuad);
		ObjectBuffer->UpdateObject(Fog);
	}
	
	//level values
	LevelFill = 100 * FlowObject->GetSize().y / LevelSize.y;
	LevelProgress = sqrt((HeroCenter.x - ExitCenter.x) * (HeroCenter.x - ExitCenter.x) + (HeroCenter.y - ExitCenter.y) * (HeroCenter.y - ExitCenter.y));
	Interface->SetStatValue(1, LevelProgress);
	Interface->SetStatValue(2, LevelFill);
	
	if(FlowObject->GetVertex().p[2][1] >= Hero->GetVertex().p[2][1])
	{
		LevelNumber = 0;
		SetState(STT_LOSE);
	}
	if(ExitObject->GetCollided()) SetState(STT_WIN);
}

void MStage::DrawBackground()
{
	Background->Draw();
}
void MStage::DrawLevel()
{
	Level2->Draw();
}
void MStage::DrawOther()
{
	ObjectBuffer->DrawObject(ExitObject);
	for(unsigned int i=0; i<Items.size(); i++)
		ObjectBuffer->DrawObject(Items[i]);
	ObjectBuffer->DrawObject(Hero);
}

void MStage::DrawFlow()
{
	ObjectBuffer->DrawObject(FlowObject);
}

void MStage::DrawInterface()
{
	Interface->Draw();
}

void MStage::DrawFog()
{
	glColorMask(true, true, true, true);
	glStencilFunc(GL_EQUAL, 1, 1);
	glAlphaFunc(GL_GREATER, 0.5);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		ObjectBuffer->DrawObject(Fog);
}

void MStage::DrawNoFog()
{
	glColorMask(false, false, false, false);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glAlphaFunc(GL_GREATER, 0.5);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		ObjectBuffer->DrawObject(NoFog);
}

glm::vec2 MStage::GetHeroCenter()
{
	return HeroCenter;
}

glm::vec2 MStage::GetHeroStartCenter()
{
	return HeroStartCenter;
}

glm::vec2 MStage::GetLevelCenter()
{
	return glm::vec2(LevelSize.x * 0.5f, LevelSize.y * 0.5f);
}

void MStage::Close()
{
	SetState(STT_CLOSE);
	
	Stop();
	FrameTimerMap.clear();
	
	LevelNumber = 0;
	
	ClearItems();
	
	if(Fog) delete Fog;
	if(NoFog) delete NoFog;
	if(Background)
	{
		Background->Close();
		delete Background;
	}
	if(ObjectBuffer)
	{
		ObjectBuffer->Close();
		delete ObjectBuffer;
	}
	if(FlowObject) delete FlowObject;
	if(ExitObject)
	{
		ExitObject->RemovePhysics();
		delete ExitObject;
	}
	if(Hero)
	{
		Hero->RemovePhysics();
		delete Hero;
	}
	if(Level2)
	{
		Level2->Close();
		delete Level2;
	}
	if(Interface)
	{
		Interface->Close();
		delete Interface;
	}
	if(World) delete World;
}

void MStage::Size(float Width, float Height)
{
	//size fog/nofog
	SetQuad(FogQuad, 0, 0, Width, Height);
	Fog->SetVertex(FogQuad);
	ObjectBuffer->UpdateObject(Fog);
	//size interface
	Interface->Size(Width, Height);
}

bool MStage::IsItemCollected()
{
	return ItemCollected;
}

void MStage::SetFogEnabled(bool Enabled)
{
	FogEnabled = Enabled;
}

bool MStage::GetFogEnabled()
{
	return FogEnabled;
}
