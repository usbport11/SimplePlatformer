#ifndef itemcontrollerH
#define itemcontrollerH

#include "ObjectBuffer.h"
#include "Level2.h"
#include "Item.h"
#include "MovableObject.h"
#include "FlowObject.h"
#include "Timer2.h"

class MItemController;
typedef void (MItemController::*ItemFunc) (unsigned int);

class MItemController
{
private:
	//pointers
	MFlowObject* pFlowObject;
	MMovableObject* pMovableObject;
	MLevel2* pLevel2;
	MObjectBuffer* pObjectBuffer;
	float* pDarkShift;

	//items array for new items and collected elements on level
	vector<MItem*> Items;
	vector<MItem*> CollectedItems;
	
	//buffs/debuffs timers (here will be more timers in future)
	MTimer2 tmConfuse;
	MTimer2 tmNoJump;
	MTimer2 tmFreeze;
	MTimer2 tmDark;
	MTimer2 tmFastFlow;
	MTimer2 tmSlow;
	bool TimerProcessEnabled;
	HANDLE hTimerProcess;
	friend void TimerProcess(void* Parameters);
	
	//tests
	//stTexture* ptxDebuffsIcons;
	//MObjectBuffer DebuffsBuffer;
	//vector<MObject*> AllDebuffsIcons;
	//map<unsigned int, MObject*> ActiveDebuffsIcons;
	
	//map and pointers to process functions (here will be more functions in future)
	map<int, ItemFunc> ItemProcess;
	void ProcessItemNull(unsigned int Number);
	void ProcessItemConfuse(unsigned int Number);
	void ProcessItemNoJump(unsigned int Number);
	void ProcessItemFreeze(unsigned int Number);
	void ProcessItemFastFlow(unsigned int Number);
	void ProcessItemDark(unsigned int Number);
	void ProcessItemSlow(unsigned int Number);
	//remove functions
	void RemoveItem(unsigned int Number);
	
public:
	MItemController();
	~MItemController();
	bool SetLinks(MMovableObject* inpMovableObject, MFlowObject* inpFlowObject, float* inpDarkShift);
	bool Initialize(MLevel2* inpLevel2, MObjectBuffer* inpObjectBuffer);
	bool PlaceAllItems(unsigned int ItemsCount, unsigned int AnimatedTextureId, float SizeX, float SizeY, b2World* pWorld);
	void CheckItems();
	bool RemoveAllItems();
	void Close();
	MItem* GetItem(unsigned int Number);
	unsigned int GetItemCount();
	void StopAllTimers();
	
	//tests
	//bool SetDebuffIcons(stTexture* inptxDebuffsIcons, unsigned int Count);
	//bool AddDebuff(unsigned int TypeNumber);
	//bool RemoveDebuff(unsigned int TypeNumber);
	//void ClearDebuffs();
	//void CloseDebuffs();
	//MObjectBuffer* GetDebuffsBuffer();
};

#endif
