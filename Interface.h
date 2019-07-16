#ifndef interfaceH
#define interfaceH

#define ADB_OK 1
#define ADB_EXIST 2
#define ADB_ERROR 3

#include "ObjectBuffer.h"
#include "Text.h"

class MInterface
{
private:
	//buffs processing
	glm::vec2 BuffsStart;
	glm::vec2 BuffsSize;
	unsigned int ActiveBuffsCount;
	MObjectBuffer BuffsBuffer;
	stTexture* ptxBuffs;
	vector<MObject*> Buffs;
	map<unsigned int, MObject*> NumberMap;
	map<unsigned int, MObject*>::iterator it;
	
	//ui processing
	glm::vec2* pLevelSize;
	glm::vec2 BottomPlaneSize;
	MObjectBuffer UIElements;
	MObject* BottomPlane;
	stTexture* ptxBottomPlane;
	
	//stats values
	vector<glm::vec2> LevelStatsCoords;
	vector<string> LevelStatsCaptions;
	MText txtLevelStatsCaptions;
	MText txtLevelStatsValues;
	char ConvertBuffer[5];
	
public:
	MInterface();
	~MInterface();
	bool Initialize(stTexture* inptxBuffs, unsigned int BuffsCount, stTexture* inptxBottomPlane, glm::vec2* inpLevelSize);
	int AddBuff(unsigned int TypeNumber);
	bool RemoveBuff(unsigned int TypeNumber);
	void Draw();
	void Clear();
	void Close();
	void Size(float Width, float Height);
	
	bool InitializeStats(unsigned int TextureId, float SizeX, float SizeY, float Width, float Height);
	bool SetStatValue(unsigned int Number, unsigned int Value);
	bool SetStatValue(unsigned int Number, float Value);
	void DrawStats();
};

#endif
