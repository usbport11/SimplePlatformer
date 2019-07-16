#ifndef itemH
#define itemH

#include "PhysicObject.h"

#define ITEM_TYPES_COUNT 6

#define IT_SLOW 0
#define IT_DARK 1
#define	IT_FASTFLOW 2
#define IT_FREEZE 3
#define IT_NOJUMP 4
#define IT_CONFUSE 5

union unItemParameter
{
	int nValue;
	float fValue;
};

class MItem: public MPhysicObject
{
private:
	bool Collected;
	unsigned int Type;
	vector<unItemParameter> ItemParameters;
	int GetEntityType();
	void OnBeginCollideWith(MPhysicObject* pObject);
public:
	MItem(unsigned int inType);
	~MItem();
	bool GetCollected();
	void AddParameter(int Value);
	void AddParameter(float Value);
	unItemParameter GetParameter(unsigned int Number);
	unsigned int GetType();
};

#endif
