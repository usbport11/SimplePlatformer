#ifndef stageH
#define stageH

#include "ActivatableObject.h"
#include "ObjectContactListener.h"
#include "ObjectBuffer.h"
#include "Level2.h"
#include "FlowObject.h"
#include "Hero.h"
#include "Interface.h"
#include "Background3.h"

#define STT_UNKNOWN 0
#define STT_INIT 1
#define STT_PREPARE 2
#define STT_READY 3
#define STT_LOSE 4
#define STT_WIN 5
#define STT_CLOSE 6

#define TX_ITEMS 0
#define TX_HERO 1
#define TX_FLOW 2
#define TX_LEVEL 3
#define TX_EXIT 4
#define TX_BACKGROUND3 5
#define TX_FOG 6
#define TX_NOFOG 7
#define TX_UIBOTTOM 8
#define TX_FONT 9

class MStage
{
private:
	//physic
	b2Vec2 Gravity;
	float32 timeStep;
	int velocityIterations;
	int positionIterations;
	b2World* World;
	
	//init values
	glm::vec2 MovableObjectSize;
	glm::vec2 ItemSize;
	glm::vec2 LevelUnitSize;
	glm::vec2 LevelSize;
	glm::vec4* pLiquidColor;
	glm::vec2 HeroStartCenter;
	glm::vec2 ExitCenter;
	
	//logic
	unsigned int LevelTypesCount;
	unsigned char State;
	unsigned int LevelNumber;
	float LevelProgress;
	float LevelFill;
	float DarkShift;
	
	//textures
	vector<stTexture*>* pTextures;
	
	//items
	bool ItemCollected;
	vector<MItem*> Items;
	vector<MItem*> CollectedItems;
	bool PlaceItems(unsigned int ItemsCount);
	void CheckItems();
	bool RemoveItem(unsigned int Number);
	
	//objects
	MObjectContactListener OCL;
	MHero* Hero;
	MFlowObject* FlowObject;
	MActivatableObject* ExitObject;
	MLevel2* Level2;
	MObjectBuffer* ObjectBuffer;
	MInterface* Interface;
	MBackground3* Background;
	MObject* Fog;
	MObject* NoFog;
	
	//fog
	stQuad FogQuad;
	glm::vec2 HeroCenter;
	glm::vec2 FogSpeed;
	bool FogEnabled;
	
	//stage state
	void SetState(unsigned char inState);
	
	//frame control
	stTimer FrameTimer;
	static void CALLBACK FrameTimerFunction(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	
public:
	MStage();
	~MStage();
	
	void Start();
	void Stop();
	
	bool Initialize(vector<stTexture*>* inpTextures, stTexture* pInterfaceBuffs, unsigned int InterfaceBuffsCount, HWND hWnd, glm::vec4* inpLiquidColor);
	bool Next();
	void ClearItems();
	bool CanStartNextLevel();
	unsigned char GetState();
	void WorldStep();
	void DrawBackground();
	void DrawLevel();
	void DrawOther();
	void DrawFlow();
	void DrawInterface();
	void DrawFog();
	void DrawNoFog();
	glm::vec2 GetHeroCenter();
	glm::vec2 GetHeroStartCenter();
	glm::vec2 GetLevelCenter();
	void Close();
	void Size(float Width, float Height);
	bool IsItemCollected();
	
	void SetFogEnabled(bool Enabled);
	bool GetFogEnabled();
};

#endif
