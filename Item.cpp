#include "stdafx.h"
#include "Item.h"

MItem::MItem(unsigned int inType):MPhysicObject()
{
	Collected = false;
	Type = inType;
}

MItem::~MItem()
{
	ItemParameters.clear();
}

int MItem::GetEntityType()
{
	return OT_COLLECTABLE;
}

void MItem::OnBeginCollideWith(MPhysicObject* pObject)
{
	Collected = true;
	cout<<"Collision with item!"<<endl;
}

void MItem::AddParameter(int Value)
{
	unItemParameter AddParameter;
	AddParameter.nValue = Value;
	ItemParameters.push_back(AddParameter);
}

void MItem::AddParameter(float Value)
{
	unItemParameter AddParameter;
	AddParameter.fValue = Value;
	ItemParameters.push_back(AddParameter);
}

unItemParameter MItem::GetParameter(unsigned int Number)
{
	return ItemParameters[Number];
}

bool MItem::GetCollected()
{
	return Collected;
}

unsigned int MItem::GetType()
{
	return Type;
}

