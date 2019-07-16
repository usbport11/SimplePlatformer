#include "stdafx.h"
#include "MovableObject.h"

MMovableObject::MMovableObject():MPhysicObject()
{
	FootFixture = NULL;
	FootContacts = 0;
	HeadFixture = NULL;
	HeadContacts = 0;
	
	MaxVelocity = 0;
	VelocityImpulse = 0;
	JumpImpulse = 0;
	KeyLeft = VK_LEFT;
	KeyRight = VK_RIGHT;
	KeyJump = VK_UP;
	MoveEnabled = true;
	JumpEnabled = true;
	PrevKeyState = CurrKeyState = 0x0000;
	
	AnimationControl = false;
}

MMovableObject::~MMovableObject()
{
	FootFixture = NULL;
	HeadFixture = NULL;
	
	MaxVelocity = 0;
	VelocityImpulse = 0;
	JumpImpulse = 0;
	MoveEnabled = false;
	PrevKeyState = CurrKeyState = 0x0000;
}

bool MMovableObject::SetMoveParameters(float inVelocityImpulse, float inJumpImpulse, float inMaxVelocity)
{
	if(!pWorld)
	{
		LogFile<<"MMovableObject: NULL world"<<endl;
		return false;
	}
	if(!Body)
	{
		LogFile<<"MMovableObject: NULL body"<<endl;
		return false;
	}
	if(!inVelocityImpulse)
	{
		LogFile<<"MMovableObject: Velocity value is NULL"<<endl;
		return false;
	}
	if(inMaxVelocity <= 0 || inMaxVelocity < inVelocityImpulse)
	{
		LogFile<<"PhysicObject: Velocity values are wrong"<<endl;
		return false;
	}
	if(BodyDef.type == b2_staticBody)
	{
		LogFile<<"MMovableObject: Can't move static object"<<endl;
		return false;
	}
	if((HalfSize.x > -0.01 && HalfSize.x < 0.01) || (HalfSize.y > -0.01 && HalfSize.y < 0.01))
	{
		LogFile<<"MMovableObject: Wrong half size"<<endl;
		return false;
	}
	
	MaxVelocity = inMaxVelocity;
	VelocityImpulse = inVelocityImpulse;
	JumpImpulse = inJumpImpulse;
	
	if(!FootFixture)
	{
		//PolygonShape.SetAsBox(HalfSize.x - HalfSize.x * 0.1f, 0.1f, b2Vec2(0.0f, 0.0f - 0.5f), 0);HalfSize.x - Edge.x
		PolygonShape.SetAsBox(HalfSize.x - Edge.x, 0.1f, b2Vec2(0.0f, 0.0f - 0.5f), 0);
		FootFixtureDef.shape = &PolygonShape;
		FootFixtureDef.isSensor = true;
		FootFixture = Body->CreateFixture(&FootFixtureDef);
		FootFixture->SetUserData((void*)FOOT_LISTENER_ID);
	}
	if(!HeadFixture)
	{
		PolygonShape.SetAsBox(HalfSize.x - Edge.x, 0.1f, b2Vec2(0.0f, HalfSize.y * 2.0f), 0);//danger: if edge is 0 we can't jump in some cases
		HeadFixtureDef.shape = &PolygonShape;
		HeadFixtureDef.isSensor = true;
		HeadFixture = Body->CreateFixture(&HeadFixtureDef);
		HeadFixture->SetUserData((void*)HEAD_LISTENER_ID);
	}
	
	return true;
}

void MMovableObject::Move()
{	
	//get velocity
    Velocity = Body->GetLinearVelocity();
    
	//cap velocity
    if(fabs(Velocity.x) > MaxVelocity)
	{			
		Velocity.x = sgn(Velocity.x) * MaxVelocity;
		Body->SetLinearVelocity(Velocity);
	}
	
	//stand still check
	if(!(GetAsyncKeyState(KeyLeft) & 0x8000) && !(GetAsyncKeyState(KeyRight) & 0x8000))
	{
		Velocity.x *= 0.9;
		Body->SetLinearVelocity(Velocity);
	}
	
	if(JumpEnabled)
	{
		CurrKeyState = GetAsyncKeyState(KeyJump);
		if(PrevKeyState == 0x0000 && CurrKeyState == 0x8001) StartJump();
		PrevKeyState = CurrKeyState;
	}
	
	//round velocity ?
	//Velocity.x = round(Velocity.x * 100.0f) * 0.01f;
	
	if(MoveEnabled)
	{
		//add impulse
	    if((GetAsyncKeyState(KeyLeft) & 0x8000) && Velocity.x > -MaxVelocity) 
		{
			Body->ApplyLinearImpulse(b2Vec2(-VelocityImpulse, 0), Body->GetWorldCenter(), true);
			if(DirectionRight) DirectionRight = false;
		}
	    if((GetAsyncKeyState(KeyRight) & 0x8000) && Velocity.x < MaxVelocity)
		{
			Body->ApplyLinearImpulse(b2Vec2(VelocityImpulse, 0), Body->GetWorldCenter(), true);
			if(!DirectionRight) DirectionRight = true;
		}
	}
	
	//animation control
	if(AnimationControl)
	{
		if(!Velocity.x && !Velocity.y) 
		{
			SetCurrentAnimation(ANM_STAY);
			return;
		}
		if(Velocity.y) SetCurrentAnimation(ANM_JUMP);
		else
			if(Velocity.x) SetCurrentAnimation(ANM_MOVE);
	}
	else if(!Velocity.x && !Velocity.y) return;
	
	//relocate data in draw buffer (based on physic size)
	Position = Body->GetPosition();
	UVQuad.p[0][0] = glm::vec2(Position.x - HalfSize.x - Edge.x, Position.y - HalfSize.y - Edge.y);
	UVQuad.p[1][0] = glm::vec2(Position.x - HalfSize.x - Edge.x, Position.y + HalfSize.y + Edge.y);
	UVQuad.p[2][0] = glm::vec2(Position.x + HalfSize.x + Edge.x, Position.y + HalfSize.y + Edge.y);
	UVQuad.p[3][0] = glm::vec2(Position.x + HalfSize.x + Edge.x, Position.y - HalfSize.y - Edge.y);
}

void MMovableObject::StartJump()
{
	if(FootContacts < 1 || HeadContacts > 0) return;
	Body->ApplyLinearImpulse(b2Vec2(0, JumpImpulse), Body->GetPosition(), true);
}

bool MMovableObject::RemovePhysics()
{
	//destroy foot fixture
	if(FootFixture) Body->DestroyFixture(FootFixture);
	if(HeadFixture) Body->DestroyFixture(HeadFixture);
	//call basic destroy
	MPhysicObject::RemovePhysics();
	
	return true;
}

bool MMovableObject::OnFixtureConnectBegin(int UserData)
{
	if(UserData == FOOT_LISTENER_ID)
	{
		FootContacts ++;
		return true;
	}
	if(UserData == HEAD_LISTENER_ID)
	{
		HeadContacts ++;
		return true;
	}
	return false;
}

bool MMovableObject::OnFixtureConnectEnd(int UserData)
{
	if(UserData == FOOT_LISTENER_ID)
	{
		FootContacts --;
		return true;
	}
	if(UserData == HEAD_LISTENER_ID)
	{
		HeadContacts --;
		return true;
	}
	return false;
}

bool MMovableObject::SetMoveKeys(int inKeyLeft, int inKeyRight, int inKeyJump)
{
	KeyLeft = inKeyLeft;
	KeyRight = inKeyRight;
	KeyJump = inKeyJump;
	return true;
}

void MMovableObject::SwapMoveKeys()
{
	int buff = KeyLeft;
	KeyLeft = KeyRight;
	KeyRight = buff;
}

void MMovableObject::SetMoveEnabled(bool Value)
{
	MoveEnabled = Value;
}

void MMovableObject::SetJumpEnabled(bool Value)
{
	JumpEnabled = Value;
}

void MMovableObject::SetAnimationControl(bool Enable)
{
	AnimationControl = Enable;
}
