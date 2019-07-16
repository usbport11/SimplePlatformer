#include "stdafx.h"
#include "ObjectContactListener.h"

void MObjectContactListener::BeginContact(b2Contact* pContact)
{
	b2Fixture* fixtureA = pContact->GetFixtureA();
	b2Fixture* fixtureB = pContact->GetFixtureB();
	MPhysicObject* pObjectA = (MPhysicObject*)fixtureA->GetBody()->GetUserData();
	MPhysicObject* pObjectB = (MPhysicObject*)fixtureB->GetBody()->GetUserData();
	if(pObjectA) pObjectA->OnFixtureConnectBegin((int)fixtureA->GetUserData());
	else 
		if(pObjectB) pObjectB->OnFixtureConnectBegin((int)fixtureB->GetUserData());
	if(pObjectA && pObjectB)
	{
		pObjectA->OnBeginCollideWith(pObjectB);
		pObjectB->OnBeginCollideWith(pObjectA);
		return;
	}
}
  
void MObjectContactListener::EndContact(b2Contact* pContact)
{
	b2Fixture* fixtureA = pContact->GetFixtureA();
	b2Fixture* fixtureB = pContact->GetFixtureB();
	MPhysicObject* pObjectA = (MPhysicObject*)fixtureA->GetBody()->GetUserData();
	MPhysicObject* pObjectB = (MPhysicObject*)fixtureB->GetBody()->GetUserData();
	if(pObjectA) pObjectA->OnFixtureConnectEnd((int)fixtureA->GetUserData());
	else
		if(pObjectB) pObjectB->OnFixtureConnectEnd((int)fixtureB->GetUserData());
	if(pObjectA && pObjectB) 
	{
		pObjectA->OnEndCollideWith(pObjectB);
		pObjectB->OnEndCollideWith(pObjectA);
		return;
	}
}
