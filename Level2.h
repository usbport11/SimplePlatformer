#ifndef level2H
#define level2H

#define FRICTION_SIZE 0.0f

struct stUnitPoint
{
	int p[2];
};

struct stUnit
{
	stUnitPoint Position;
	stUnitPoint Size;
};

class MLevel2
{
private:
	b2World* pWorld;
	b2BodyDef BoxBodyDef;
	b2Body* BoxBody;
	b2PolygonShape Box;
	vector<b2Fixture*> Fixtures;//pointer to fixture, need for merging

	bool Ready;
	glm::vec2 UnitSize; //points per unit
	glm::vec2 LevelOffset; //offset, if needed
	stUnitPoint EdgesOffset; //level edges offset
	stUnitPoint BoxLimit[2]; //x,y size limits
	stUnitPoint HoleLimit; //hole size
	stUnitPoint TotalUnit; //total units count in level
	
	bool WasHole;
	bool WasConnect;
	stUnit LastBox;
	stUnit PrevBox; //for merging
	stUnit HighestBox; //for creating connect box
	stUnitPoint CurrentUnit; //current unit pos
	stUnitPoint CurrentLimit; //limits used in conditions while line generating
	stUnitPoint BoxUnitSize; //generated box size
	unsigned int BoxNumber; //number of box need to make pair and add to quad tree
	
	void AddEdge(int PosX, int PosY, int SizeX, int SizeY);
	void GenerateConnect();
	void GenerateBox();
	void SetBufferData();
	void GenerateHole();
	void GenerateLine();
	bool GenerateLevel();
	stQuad GetBoxQuadByNumber(unsigned int Number);
	
	//opengl data
	GLuint BaseVertexBufferId;
	GLuint BaseUVBufferId;
	GLuint OffsetVertexBufferId;
	GLuint OffsetUVBufferId;
	
	glm::vec2 BaseVertex[4];
	glm::vec2 BaseUV[4];
	vector<glm::vec2> OffsetVertex;
	vector<glm::vec2> OffsetUV;
	
	//values for calculate boxe size and center
	glm::vec2 Center;
	glm::vec2 HalfSize;
	stQuad VertexAdd;
	
	glm::vec2 UVUnit;
	unsigned int TextureId;
	unsigned int TexureSize[2];
	unsigned int SubCount[2];
	unsigned int OffsetCount;
	
	unsigned int LevelTypeOffset;
	
public:
	MLevel2();
	~MLevel2();
	bool Initialize(b2World* inpWorld);
	bool SetTextureData(unsigned int inTextureId, unsigned int Width, unsigned int Height, unsigned int SubCountX, unsigned int SubCountY);
	
	bool SetUnit(float UnitSizeX, float UnitSizeY, unsigned int TotalUnitX, unsigned int TotalUnitY);
	bool SetBoxLimits(unsigned int UnitLimitX0, unsigned int UnitLimitX1, unsigned int UnitLimitY0, unsigned int UnitLimitY1);
	bool SetHoleLimit(unsigned int UnitLimitX0, unsigned int UnitLimitX1);
	bool SetEdgesOffsets(int UnitOffsetX, int UnitOffsetY);
	bool Create(unsigned int inLevelTypeOffset);
	bool CreateTest();
	bool GetReady();
	void Draw();
	void Clear();
	void Close();
	
	glm::vec2 GetUnitSize();
	stQuad GetFirstBoxQuad();
	stQuad GetLastBoxQuad();
	unsigned int GetRandomBoxQuad(stQuad& RetQuad);
	unsigned int GetBoxesCount();
};

#endif
