#ifndef objectcontactlistenerH
#define objectcontactlistenerH

#include "PhysicObject.h"

class MObjectContactListener: public b2ContactListener
{
private:
	void BeginContact(b2Contact* pContact);
	void EndContact(b2Contact* pContact);
};

#endif
