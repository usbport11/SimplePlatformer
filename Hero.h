#ifndef heroH
#define heroH

#include "Timer2.h"
#include "Item.h"
#include "MovableObject.h"
#include "Interface.h"

class MHero: public MMovableObject
{
private:
	MInterface* pInterface;
	map<unsigned int, MTimer2> Timers;//precreated map with all debuff types
	map<unsigned int, MTimer2>::iterator itTimers;
	vector<unsigned int> ActiveTimers;//active debuff timers
	vector<unsigned int>::iterator itActiveTimers;
	void Action(unsigned int TypeNumber, bool Start);
	void OnBeginCollideWith(MPhysicObject* pObject);
public:
	MHero();
	~MHero();
	bool InitTimers(unsigned int BuffsCount);
	bool SetInterface(MInterface* inpInterface);
	void ClearActiveTimers();
	void TimeCheck();
	int GetEntityType();
};

#endif
