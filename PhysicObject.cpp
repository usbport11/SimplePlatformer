#include "stdafx.h"
#include "PhysicObject.h"

MPhysicObject::MPhysicObject():MAnimatedObject()
{
	pWorld = NULL;
	Body = NULL;
	BodyFixture = NULL;
	Edge = b2Vec2(0, 0);
}

MPhysicObject::~MPhysicObject()
{
	pWorld = NULL;
	Body = NULL;
	BodyFixture = NULL;
}

bool MPhysicObject::AddPhysics(b2World* inpWorld, b2BodyType inBodyType, bool inSensorFixture, bool inBullet, uint16 inFilterCategory, uint16 inFilterMask, b2Vec2 inEdge)
{
	if(!inpWorld)
	{
		LogFile<<"PhysicObject: NULL world"<<endl;
		return false;
	}
	glm::vec2 QuadSize = GetQuadSize(GetVertexQuad(UVQuad));
	if(QuadSize.x <= 0.01f || QuadSize.y <= 0.01f)
	{
		LogFile<<"PhysicObject: Too small quad size (not initialized?)"<<endl;
		return false;
	}
	pWorld = inpWorld;
	BodyType = inBodyType;
	SensorFixture = inSensorFixture;
	Bullet = inBullet;
	FilterCategory = inFilterCategory;
	FilterMask = inFilterMask;
	Edge = inEdge;

	HalfSize = b2Vec2((QuadSize.x - Edge.x * 2) * 0.5f, (QuadSize.y - Edge.y * 2) * 0.5f);
	if(HalfSize.x <= 0 || HalfSize.y <= 0)
	{
		LogFile<<"PhysicObject: Wrong half size"<<endl;
		return false;
	}
	BodyDef.type = BodyType;
	BodyDef.fixedRotation = true;
	BodyDef.position.Set(UVQuad.p[0][0].x + HalfSize.x, UVQuad.p[0][0].y + HalfSize.y);
	PolygonShape.SetAsBox(HalfSize.x, HalfSize.y);
	FixtureDef.isSensor = SensorFixture;
	FixtureDef.shape = &PolygonShape;
	FixtureDef.density = 1.0f;
	//FixtureDef.friction = 3.0f;//?
	FixtureDef.filter.categoryBits = FilterCategory;
	FixtureDef.filter.maskBits = FilterMask;
	Body = pWorld->CreateBody(&BodyDef);
	BodyFixture = Body->CreateFixture(&FixtureDef);
	Body->SetBullet(Bullet);
	Body->SetUserData(this);
	
	/*
	b2PolygonShape* lPolygonShape = (b2PolygonShape*)BodyFixture->GetShape();
	cout<<"(0,0): ["<<lPolygonShape->GetVertex(0).x<<", "<<lPolygonShape->GetVertex(0).y
		<<"] (0,1): ["<<lPolygonShape->GetVertex(1).x<<", "<<lPolygonShape->GetVertex(1).y
		<<"] (1,1): ["<<lPolygonShape->GetVertex(2).x<<", "<<lPolygonShape->GetVertex(2).x
		<<"] (1,0): ["<<lPolygonShape->GetVertex(3).x<<", "<<lPolygonShape->GetVertex(3).x<<"] VC: "<<lPolygonShape->GetVertexCount()<<endl;
	*/
	b2AABB aabb;
	aabb = BodyFixture->GetAABB(0);
	cout<<"AABB: lower: "<<aabb.lowerBound.x<<" "<<aabb.lowerBound.y<<" upper: "<<aabb.upperBound.x<<" "<<aabb.upperBound.y<<endl;
	//test data
	b2Vec2 Position = Body->GetPosition();
	cout<<" (0,0): ["<<Position.x - HalfSize.x - Edge.x<<", "<<Position.y - HalfSize.y - Edge.y<<
		"] (0,1): ["<<Position.x - HalfSize.x - Edge.x<<", "<<Position.y + HalfSize.y + Edge.y<<
		"] (1,1): ["<<Position.x + HalfSize.x + Edge.x<<", "<<Position.y + HalfSize.y + Edge.y<<
		"] (1,0): ["<<Position.x + HalfSize.x + Edge.x<<", "<<Position.y - HalfSize.y - Edge.y<<"]"<<endl;
	
	return true;
}

bool MPhysicObject::UpdatePhysics()
{
	if(!Body || !pWorld)
	{
		LogFile<<"PhysicObject: Wolrd or Body is NULL"<<endl;
		return false;
	}
	Body->SetTransform(b2Vec2(GetQuadCenter(GetVertexQuad(UVQuad)).x, GetQuadCenter(GetVertexQuad(UVQuad)).y), 0.0f);
	return true;
}

bool MPhysicObject::RemovePhysics()
{
	//stop physics
	if(Body) Body->SetActive(false);
	//body fixture destroy
	if(BodyFixture) Body->DestroyFixture(BodyFixture);
	//destroy body
	if(Body) pWorld->DestroyBody(Body);
	
	Body = NULL;
	BodyFixture = NULL;
	
	return true;
}

void MPhysicObject::PhysicEnable(bool Enable)
{
	if(Body) Body->SetActive(Enable);
}

bool MPhysicObject::OnFixtureConnectBegin(int UserData){}
bool MPhysicObject::OnFixtureConnectEnd(int UserData){}
void MPhysicObject::OnBeginCollide(){}
void MPhysicObject::OnEndCollide(){}
int MPhysicObject::GetEntityType(){return OT_BOUNDARY;}
void MPhysicObject::OnBeginCollideWith(MPhysicObject* pObject){}
void MPhysicObject::OnEndCollideWith(MPhysicObject* pObject){}
