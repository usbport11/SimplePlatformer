#ifndef movableobjectH
#define movableobjectH

#include "PhysicObject.h"

#define FOOT_LISTENER_ID 3
#define HEAD_LISTENER_ID 4

#define ANM_STAY 0
#define ANM_MOVE 1
#define ANM_JUMP 2
#define ANM_DEAD 3

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

class MMovableObject: public MPhysicObject
{
private:
	//foot
	int FootContacts;
	b2FixtureDef FootFixtureDef;
	b2Fixture* FootFixture;
	//head
	int HeadContacts;
	b2FixtureDef HeadFixtureDef;
	b2Fixture* HeadFixture;
	
	float JumpImpulse;
	float VelocityImpulse;
	float MaxVelocity;
	
	b2Vec2 Velocity;
	b2Vec2 Position;
	
	WORD CurrKeyState;
	WORD PrevKeyState;
	bool MoveEnabled;
	bool JumpEnabled;
	int KeyLeft;
	int KeyRight;
	int KeyJump;
	
	bool AnimationControl;
public:
	MMovableObject();
	~MMovableObject();
	bool SetMoveKeys(int inKeyLeft, int inKeyRight, int inKeyJump);
	bool SetMoveParameters(float inVelocityImpulse, float inJumpImpulse, float inMaxVelocity);
	void Move();
	void StartJump();
	bool RemovePhysics();
	
	void SwapMoveKeys();
	void SetMoveEnabled(bool Value);
	void SetJumpEnabled(bool Value);
	
	bool OnFixtureConnectBegin(int UserData);
	bool OnFixtureConnectEnd(int UserData);
	
	void SetAnimationControl(bool Enable);
};

#endif
