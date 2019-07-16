#ifndef activatableobjectH
#define activatableobjectH

#include "PhysicObject.h"

enum enActivatableObjectType
{
	AOT_EXIT = 1,
	AOT_LIGHT = 2,
};

class MActivatableObject: public MPhysicObject
{
private:
	bool Collided;
	bool Activated;
	bool CanInverse;
	enActivatableObjectType Type;
	int GetEntityType();
	void OnBeginCollideWith(MPhysicObject* pObject);
	void OnEndCollideWith(MPhysicObject* pObject);
public:
	MActivatableObject(enActivatableObjectType ActivatableObjectType);
	enActivatableObjectType GetType();
	bool Activate();
	void SetCanInverse(bool inCanInverse);
	void Inverse();
	bool GetCollided();
	bool GetActivated();
};

#endif
