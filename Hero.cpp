#include "stdafx.h"
#include "Hero.h"

MHero::MHero():MMovableObject()
{
	pInterface = NULL;
}

MHero::~MHero()
{
	pInterface = NULL;
	Timers.clear();
	ActiveTimers.clear();
}

int MHero::GetEntityType()
{
	return OT_MOVABLE;
}

void MHero::OnBeginCollideWith(MPhysicObject* pObject)
{
	if(!pObject)
	{
		cout<<"Null object"<<endl;
		return;
	}
	if(pObject->GetEntityType() == OT_COLLECTABLE)
	{
		MItem* pItem = (MItem*)pObject;
		if(!pItem)
		{
			cout<<"Not item"<<endl;
			return;
		}
		unsigned int ItemType = pItem->GetType();
		
		//this function not sync with timers. it is dungerous, needs tests
		//add icon to interface
		switch(pInterface->AddBuff(ItemType))
		{
			case ADB_OK:
				Action(ItemType, true);
				cout<<"Start action"<<endl;
				ActiveTimers.push_back(ItemType);
				Timers[ItemType].Start(pItem->GetParameter(0).nValue);
				cout<<"Timer value: "<<pItem->GetParameter(0).nValue<<endl;
				break;
			case ADB_EXIST:
				Timers[ItemType].Start(pItem->GetParameter(0).nValue);
				cout<<"Restart existed timer"<<endl;
				break;
			case ADB_ERROR:
				cout<<"Some error"<<endl;
				break;
		}
	}
}

void MHero::Action(unsigned int TypeNumber, bool Start)
{
	switch(TypeNumber)
	{
		case IT_CONFUSE:
			SwapMoveKeys();
			break;
		case IT_NOJUMP:
			if(Start) SetJumpEnabled(false);
			else SetJumpEnabled(true);
			break;
		case IT_FREEZE:
			if(Start)
			{
				SetMoveEnabled(false);
				SetJumpEnabled(false);
			}
			else
			{
				SetMoveEnabled(true);
				SetJumpEnabled(true);
			}
			break;
		case IT_FASTFLOW:
			break;
		case IT_DARK:
			break;
		case IT_SLOW:
			if(Start) SetMoveParameters(0.02f, 0.6f, 1.0f);
			else SetMoveParameters(0.05f, 0.6f, 2.0f);
			break;
	}
}

bool MHero::InitTimers(unsigned int BuffsCount)
{
	if(!BuffsCount) return false;
	MTimer2 AddTimer;
	for(unsigned int i=0; i<BuffsCount; i++)
	{
		AddTimer.Create(i, 0);
		Timers.insert(pair<unsigned int, MTimer2>(i, AddTimer));
	}
	
	return true;
}

bool MHero::SetInterface(MInterface* inpInterface)
{
	if(!inpInterface) return false;
	pInterface = inpInterface;
	return true;
}

void MHero::TimeCheck()
{
	if(!Timers.size()) return;
	for(itTimers=Timers.begin(); itTimers!=Timers.end(); ++itTimers)
	{
		if(itTimers->second.Time())
		{
			Action(itTimers->first, false);
			itActiveTimers = find(ActiveTimers.begin(), ActiveTimers.end(), itTimers->first);
			if(itActiveTimers != ActiveTimers.end())
			{
				ActiveTimers.erase(itActiveTimers);
				//remove icon from interface
				if(pInterface) pInterface->RemoveBuff(itTimers->first);
			}
		}
	}
}

void MHero::ClearActiveTimers()
{
	ActiveTimers.clear();
}
