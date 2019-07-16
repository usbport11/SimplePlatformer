#include "stdafx.h"
#include "ItemController.h"

void TimerProcess(void* Parameters)
{
	MItemController* pItemController = (MItemController*)Parameters;
	if(!pItemController) return;
	LogFile<<"ItemController: Thread timers started"<<endl;
	while(pItemController->TimerProcessEnabled)
	{
		if(pItemController->tmConfuse.Time())
		{
			cout<<"Timer Confuse stop"<<endl;
			pItemController->pMovableObject->SwapMoveKeys();
			//pItemController->RemoveDebuff(IT_CONFUSE);
		}
		if(pItemController->tmFreeze.Time())
		{
			cout<<"Timer Freeze stop"<<endl;
			pItemController->pMovableObject->SetMoveEnabled(true);
			pItemController->pMovableObject->SetJumpEnabled(true);
			//pItemController->RemoveDebuff(IT_FREEZE);
		}
		if(pItemController->tmNoJump.Time())
		{
			cout<<"Timer NoJump stop"<<endl;
			pItemController->pMovableObject->SetJumpEnabled(true);
			//pItemController->RemoveDebuff(IT_NOJUMP);
		}
		if(pItemController->tmSlow.Time())
		{
			cout<<"Timer Slow stop"<<endl;
			pItemController->pMovableObject->SetMoveParameters(0.05f, 0.9f, 2.0f);
			//pItemController->RemoveDebuff(IT_SLOW);
		}
		if(pItemController->tmDark.Time())
		{
			cout<<"Timer Dark stop"<<endl;
			*pItemController->pDarkShift = 0.0f;
			//pItemController->RemoveDebuff(IT_DARK);
		}
		if(pItemController->tmFastFlow.Time())
		{
			cout<<"Timer FastFlow stop"<<endl;
			pItemController->pFlowObject->SetSizeVelocity(0, 0.16);
			//pItemController->RemoveDebuff(IT_FASTFLOW);
		}
		Sleep(5);
	}
	LogFile<<"ItemController: Thread timers closed"<<endl;
}

MItemController::MItemController()
{
	pLevel2 = NULL;
	pObjectBuffer = NULL;
	pMovableObject = NULL;
	pFlowObject = NULL;
	pDarkShift = NULL;
}

MItemController::~MItemController()
{
	pLevel2 = NULL;
	pObjectBuffer = NULL;
	pMovableObject = NULL;
	pFlowObject = NULL;
	pDarkShift = NULL;
}

bool MItemController::SetLinks(MMovableObject* inpMovableObject, MFlowObject* inpFlowObject, float* inpDarkShift)
{
	if(!inpMovableObject)
	{
		LogFile<<"MItemController: can not link MovableObject"<<endl;
		return false;
	}
	if(!inpFlowObject)
	{
		LogFile<<"MItemController: can not link FlowObject"<<endl;
		return false;
	}
	if(!inpDarkShift)
	{
		LogFile<<"MItemController: can not link DarkShift"<<endl;
		return false;
	}
	pMovableObject = inpMovableObject;
	pFlowObject = inpFlowObject;
	pDarkShift = inpDarkShift;
	return true;
}

bool MItemController::Initialize(MLevel2* inpLevel2, MObjectBuffer* inpObjectBuffer)
{
	if(!inpLevel2)
	{
		LogFile<<"ItemController: NULL Level"<<endl;
		return false;
	}
	if(!inpObjectBuffer)
	{
		LogFile<<"ItemController: NULL Object buffer"<<endl;
		return false;
	}
	pLevel2 = inpLevel2;
	pObjectBuffer = inpObjectBuffer;
	
	//create timers
	tmConfuse.Create(TM_CONFUSE, 0);
	tmFreeze.Create(TM_FREEZE, 0);
	tmNoJump.Create(TM_NOJUMP, 0);
	tmSlow.Create(TM_SLOW, 0);
	tmDark.Create(TM_DARK, 0);
	tmFastFlow.Create(TM_FASTFLOW, 0);
	//start timers thread
	TimerProcessEnabled = true;
	hTimerProcess = (HANDLE)_beginthread(TimerProcess, 0, this);
	
	//set functions pointers
	ItemProcess.insert(pair<int, ItemFunc>(IT_NONE, &MItemController::ProcessItemNull));
	ItemProcess.insert(pair<int, ItemFunc>(IT_CONFUSE, &MItemController::ProcessItemConfuse));
	ItemProcess.insert(pair<int, ItemFunc>(IT_NOJUMP, &MItemController::ProcessItemNoJump));
	ItemProcess.insert(pair<int, ItemFunc>(IT_FREEZE, &MItemController::ProcessItemFreeze));
	ItemProcess.insert(pair<int, ItemFunc>(IT_FASTFLOW, &MItemController::ProcessItemFastFlow));
	ItemProcess.insert(pair<int, ItemFunc>(IT_DARK, &MItemController::ProcessItemDark));
	ItemProcess.insert(pair<int, ItemFunc>(IT_SLOW, &MItemController::ProcessItemSlow));
	
	return true;
}

bool MItemController::PlaceAllItems(unsigned int ItemsCount, unsigned int AnimatedTextureId, float SizeX, float SizeY, b2World* pWorld)
{
	if(!ItemsCount)
	{
		LogFile<<"ItemController: NULL Items count"<<endl;
		return false;
	}
	if(!AnimatedTextureId)
	{
		LogFile<<"ItemController: NULL Items Texture Id"<<endl;
		return false;
	}

	bool* UsedBoxes = new bool [pLevel2->GetBoxesCount()];
	memset(UsedBoxes, 0, pLevel2->GetBoxesCount());
	UsedBoxes[0] = true; //disable placing items on first box
	stQuad BoxQuad;
	unsigned int RandItemType, BoxNumber;
	for(int i=0; i<ItemsCount; i++)
	{
		BoxNumber = pLevel2->GetRandomBoxQuad(BoxQuad);
		if(UsedBoxes[BoxNumber])
		{
			cout<<"Relocate item"<<endl;
			i --;
			continue;
		}
		UsedBoxes[BoxNumber] = true;
		RandItemType = (rand() % (int)ITEM_TYPES_COUNT) + 1;
		Items.push_back(new MItem(IntToItemType(RandItemType)));
		if(!pObjectBuffer->AddObject(Items[i], GetQuadCenter(BoxQuad).x - 0.16, BoxQuad.p[1].y + 0.08, SizeX, SizeY, 0, 0, 1, 1, AnimatedTextureId)) return false;
		if(!Items[i]->SetAnimations(8, 4)) return false;//ITEM_TYPES_COUNT
		if(!Items[i]->SetAnimationType(RandItemType - 1, ANMT_LOOP)) return false;
		if(!Items[i]->SetCurrentAnimation(RandItemType - 1)) return false;
		Items[i]->StartAnimation();
		if(!Items[i]->AddPhysics(pWorld, b2_kinematicBody, true, true, OT_COLLECTABLE, OT_MOVABLE)) return false;
	}
	if(UsedBoxes) delete [] UsedBoxes;
	
	return true;
}

bool MItemController::RemoveAllItems()
{
	//will be more timers
	StopAllTimers();
	//remove items (not collected and collected)
	//important to remove items in object buffer especially while regenerate level
	for(unsigned int i=0; i<Items.size(); i++)
	{
		pObjectBuffer->RemoveObject(Items[i]);
		Items[i]->RemovePhysics();
		delete Items[i];
	}
	for(unsigned int i=0; i<CollectedItems.size(); i++)
	{
		pObjectBuffer->RemoveObject(CollectedItems[i]);
		delete CollectedItems[i];
	}
	Items.clear();
	CollectedItems.clear();
	
	return true;
}

void MItemController::Close()
{
	TimerProcessEnabled = false;
	WaitForSingleObject(hTimerProcess, 5);
	ItemProcess.clear();
	//CloseDebuffs();
}

unsigned int MItemController::GetItemCount()
{
	return Items.size();
}

MItem* MItemController::GetItem(unsigned int Number)
{
	if(Number >= Items.size()) return NULL;
	return Items[Number];
}

void MItemController::CheckItems()
{
	for(int i=0; i<Items.size(); i++)
	{
		((*this).*(ItemProcess[Items[i]->GetCollected() * Items[i]->GetType()]))(i);
	}
}

void MItemController::ProcessItemNull(unsigned int Number)
{
	return;
}

void MItemController::ProcessItemConfuse(unsigned int Number)
{
	tmConfuse.Start(Items[Number]->GetItemParameters().Values[0]);
	cout<<"Timer Confuse started: "<<Items[Number]->GetItemParameters().Values[0]<<endl;
	if(pMovableObject) pMovableObject->SwapMoveKeys();
	RemoveItem(Number);
}
void MItemController::ProcessItemNoJump(unsigned int Number)
{
	tmNoJump.Start(Items[Number]->GetItemParameters().Values[0]);
	cout<<"Timer NoJump started: "<<Items[Number]->GetItemParameters().Values[0]<<endl;
	if(pMovableObject) pMovableObject->SetJumpEnabled(false);
	RemoveItem(Number);
}

void MItemController::ProcessItemFreeze(unsigned int Number)
{
	tmFreeze.Start(Items[Number]->GetItemParameters().Values[0]);
	cout<<"Timer Freeze started: "<<Items[Number]->GetItemParameters().Values[0]<<endl;
	if(pMovableObject) pMovableObject->SetMoveEnabled(false);
	if(pMovableObject) pMovableObject->SetJumpEnabled(false);
	RemoveItem(Number);
}

void MItemController::ProcessItemFastFlow(unsigned int Number)
{
	tmFastFlow.Start(Items[Number]->GetItemParameters().Values[0]);
	cout<<"Timer FastFlow started: "<<Items[Number]->GetItemParameters().Values[0]<<endl;
	if(pFlowObject) pFlowObject->SetSizeVelocity(0, 0.64);
	RemoveItem(Number);
}

void MItemController::ProcessItemDark(unsigned int Number)
{
	tmDark.Start(Items[Number]->GetItemParameters().Values[0]);
	cout<<"Timer Dark started: "<<Items[Number]->GetItemParameters().Values[0]<<endl;
	if(pDarkShift) *pDarkShift = 0.5f;
	RemoveItem(Number);
}

void MItemController::ProcessItemSlow(unsigned int Number)
{
	tmSlow.Start(Items[Number]->GetItemParameters().Values[0]);
	cout<<"Timer Slow started: "<<Items[Number]->GetItemParameters().Values[0]<<endl;
	if(pMovableObject) pMovableObject->SetMoveParameters(0.02f, 0.9f, 1.0f);
	RemoveItem(Number);
}

void MItemController::RemoveItem(unsigned int Number)
{
	cout<<"Item number: "<<Number<<" Item type: "<<Items[Number]->GetType()<<endl;
	//if(!AddDebuff(Items[Number]->GetType())) cout<<"Add to buffer fails"<<endl;
	//remove physic
	Items[Number]->PhysicEnable(false);
	//migrate to collected items vector and remove from base
	CollectedItems.push_back(Items[Number]);
	Items.erase(Items.begin() + Number);
}

void MItemController::StopAllTimers()
{
	tmConfuse.Stop();
	tmFreeze.Stop();
	tmNoJump.Stop();
	tmSlow.Stop();
	tmDark.Stop();
	tmFastFlow.Stop();
}

/*
bool MItemController::SetDebuffIcons(stTexture* inptxDebuffsIcons, unsigned int Count)
{
	if(!inptxDebuffsIcons) return false;
	if(!Count) return false;
	ptxDebuffsIcons = inptxDebuffsIcons;
	for(unsigned int i=0; i<Count; i++)
		AllDebuffsIcons.push_back(new MObject);
	if(!DebuffsBuffer.Initialize(GL_STREAM_DRAW)) return false;
	return true;
}

bool MItemController::AddDebuff(unsigned int TypeNumber)
{
	cout<<"Trying add debuff to visual buffer: "<<TypeNumber<<endl;
	if(!TypeNumber) return false;
	if(TypeNumber > AllDebuffsIcons.size()) return false;
	map<unsigned int, MObject*>::iterator it;
	it = ActiveDebuffsIcons.find(TypeNumber);
	if(it != ActiveDebuffsIcons.end()) return false;
	ActiveDebuffsIcons.insert(pair<unsigned int, MObject*>(TypeNumber, AllDebuffsIcons[TypeNumber - 1]));
	if(!DebuffsBuffer.AddObject(AllDebuffsIcons[TypeNumber - 1], ActiveDebuffsIcons.size() * 0.32f, 0.5f, 0.32f, 0.32f, 0, 0, 1, 1, ptxDebuffsIcons[TypeNumber - 1].Id)) return false;
	if(!DebuffsBuffer.DisposeAll()) return false;
	return true;
}

bool MItemController::RemoveDebuff(unsigned int TypeNumber)
{
	cout<<"Trying remove debuff from visual buffer: "<<TypeNumber<<endl;
	if(!TypeNumber) return false;
	map<unsigned int, MObject*>::iterator it;
	it = ActiveDebuffsIcons.find(TypeNumber);
	if(it == ActiveDebuffsIcons.end()) return false;
	if(!DebuffsBuffer.RemoveObject((*it).second)) return false;
	ActiveDebuffsIcons.erase(it);
	//need relocate all debuffs
	unsigned int Size = ActiveDebuffsIcons.size();
	stQuad Vertex;
	for(unsigned int i=0; i<Size; i++)
	{
		SetQuad(Vertex, i * 0.32f, 0.5f, 0.32f, 0.32f);
		ActiveDebuffsIcons[i]->SetVertex(Vertex);
		if(!DebuffsBuffer.UpdateObject(ActiveDebuffsIcons[i])) return false;
	}
	if(!DebuffsBuffer.DisposeAll()) return false;
	return true;
}

void MItemController::ClearDebuffs()
{
	ActiveDebuffsIcons.clear();
	unsigned int Size = AllDebuffsIcons.size();
	for(unsigned int i=0; i<Size; i++)
	{
		if(AllDebuffsIcons[i])
		{
			DebuffsBuffer.RemoveObject(AllDebuffsIcons[i]);
			delete AllDebuffsIcons[i];
		}
	}
	AllDebuffsIcons.clear();
}

void MItemController::CloseDebuffs()
{
	ClearDebuffs();
	DebuffsBuffer.Close();
}

MObjectBuffer* MItemController::GetDebuffsBuffer()
{
	return &DebuffsBuffer;
}
*/
