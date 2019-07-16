#include "stdafx.h"
#include "ActivatableObject.h"

MActivatableObject::MActivatableObject(enActivatableObjectType ActivatableObjectType):MPhysicObject()
{
	Type = ActivatableObjectType;
	Collided = false;
	Activated = false;
	CanInverse = false;
}

int MActivatableObject::GetEntityType()
{
	return OT_ACTIVATABLE;
}

void MActivatableObject::OnBeginCollideWith(MPhysicObject* pObject)
{
	Collided = true;
	cout<<"Start collide with ActivatableObject!"<<endl;
}

void MActivatableObject::OnEndCollideWith(MPhysicObject* pObject)
{
	Collided = false;
	cout<<"End collide with ActivatableObject!"<<endl;
}

enActivatableObjectType MActivatableObject::GetType()
{
	return Type;
}

bool MActivatableObject::Activate()
{
	if(Collided && !Activated) Activated = true;
	return Activated;
}

void MActivatableObject::SetCanInverse(bool inCanInverse)
{
	CanInverse = inCanInverse;
}

void MActivatableObject::Inverse()
{
	if(Collided && CanInverse) Activated = !Activated;
}

bool MActivatableObject::GetCollided()
{
	return Collided;
}

bool MActivatableObject::GetActivated()
{
	return Activated;
}
