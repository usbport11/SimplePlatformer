#include "stdafx.h"
#include "Level2.h"

MLevel2::MLevel2()
{
	UnitSize[0] = 0;
	UnitSize[1] = 0;
	TotalUnit.p[0] = 0;
	TotalUnit.p[1] = 0;
	BoxLimit[0].p[0] = 0;
	BoxLimit[0].p[1] = 0;
	BoxLimit[1].p[0] = 0;
	BoxLimit[1].p[1] = 0;
	HoleLimit.p[0] = 0;
	HoleLimit.p[1] = 0;
	
	BaseVertexBufferId = 0;
	BaseUVBufferId = 0;
	OffsetVertexBufferId = 0;
	OffsetUVBufferId = 0;
	TextureId = 0;
	TexureSize[0] = TexureSize[1] = 0;
	SubCount[0] = SubCount[1] = 0;
	OffsetCount = 0;
	
	EdgesOffset.p[0] = -1;
	EdgesOffset.p[1] = -1;
	
	Ready = false;
	
	pWorld = NULL;
	BoxBody = NULL;
	BoxNumber = 0;
	
	LevelTypeOffset = 0;
}

MLevel2::~MLevel2()
{
}

void MLevel2::AddEdge(int PosX, int PosY, int SizeX, int SizeY)
{
	for(int i=0; i<SizeY; i++)
	{
		for(int j=0; j<SizeX; j++)
		{
			OffsetVertex.push_back(glm::vec2((PosX + j) * UnitSize[0], (PosY + i) * UnitSize[1]));
			//OffsetUV.push_back(glm::vec2(UVUnit[0] * (rand() % SubCount[0]), UVUnit[1] * (rand() % SubCount[1])));
			OffsetUV.push_back(glm::vec2(UVUnit[0] * (rand() % SubCount[0]), UVUnit[1] * LevelTypeOffset));
			//cout<<OffsetUV[OffsetUV.size() - 1].x<<" "<<OffsetUV[OffsetUV.size() - 1].y<<endl;
		}
	}
	
	Box.SetAsBox(SizeX * 0.5 * UnitSize.x, SizeY * 0.5 * UnitSize.y, b2Vec2((PosX + SizeX * 0.5) * UnitSize.x, (PosY + SizeY * 0.5) * UnitSize.y), 0.0f);
	//BoxBody->CreateFixture(&Box, FRICTION_SIZE);
	Fixtures.push_back(BoxBody->CreateFixture(&Box, FRICTION_SIZE));
}

void MLevel2::GenerateConnect()
{
	//generate full size of box in units
	stUnitPoint RandSize;
	stUnitPoint LeftSize;
	LeftSize.p[0] = CurrentLimit.p[0] - (HighestBox.Position.p[0] + HighestBox.Size.p[0]);
	LeftSize.p[1] = CurrentLimit.p[1] - CurrentUnit.p[1];
	if(HighestBox.Size.p[0] > 2) RandSize.p[0] = rand() % (HighestBox.Size.p[0] - 1) + 1;
	else RandSize.p[0] = 1;
	RandSize.p[1] = 1;
	//fix right offset
	if(RandSize.p[0] + 1 >= HighestBox.Size.p[0]) RandSize.p[0] --;
	//size test
	if(LeftSize.p[0] < RandSize.p[0]) RandSize.p[0] = LeftSize.p[0];
	//y connect test
	if(HighestBox.Position.p[1] + HighestBox.Size.p[1] + RandSize.p[1] >= CurrentLimit.p[1]) return;
	//lastbox
	LastBox.Position.p[0] = HighestBox.Position.p[0] + 1;
	LastBox.Position.p[1] = HighestBox.Position.p[1] + HighestBox.Size.p[1] + 1;
	LastBox.Size.p[0] = RandSize.p[0];
	LastBox.Size.p[1] = RandSize.p[1];
	//if somehow size is 0
	if(LastBox.Size.p[0] <= 0) LastBox.Size.p[0] = 1;
	
	//add collision quad and fill vertex/uv buffers
	SetBufferData();
	
	HighestBox = LastBox;
}

void MLevel2::GenerateBox()
{	
	stUnitPoint RandSize;
	stUnitPoint LeftSize;
	
	//generate full size of box in units
	//box x limt
	if(BoxLimit[0].p[0] != BoxLimit[0].p[1]) RandSize.p[0] = (rand() % ((BoxLimit[0].p[1] - BoxLimit[0].p[0]) + 1)) + BoxLimit[0].p[0];
	else RandSize.p[0] = BoxLimit[0].p[0];
	//box y limit
	if(BoxLimit[1].p[0] != BoxLimit[1].p[1]) RandSize.p[1] = (rand() % ((BoxLimit[1].p[1] - BoxLimit[1].p[0]) +  1)) + BoxLimit[1].p[0];
	else RandSize.p[1] = BoxLimit[1].p[0];
	LeftSize.p[0] = CurrentLimit.p[0] - CurrentUnit.p[0];
	LeftSize.p[1] = CurrentLimit.p[1] - CurrentUnit.p[1];
	if(LeftSize.p[0] < RandSize.p[0]) RandSize.p[0] = LeftSize.p[0];
	if(LeftSize.p[1] < RandSize.p[1]) RandSize.p[1] = LeftSize.p[1];
	
	LastBox.Position.p[0] = CurrentUnit.p[0];
	LastBox.Position.p[1] = CurrentUnit.p[1];
	LastBox.Size.p[0] = RandSize.p[0];
	LastBox.Size.p[1] = RandSize.p[1];
	
	//if size y x smaller than minimum size - fix y size (it defend from set as highest box)
	if(RandSize.p[0] < BoxLimit[0].p[0]) LastBox.Size.p[1] = 1;
	
	//check on highest element
	if(LastBox.Size.p[1] > HighestBox.Size.p[1]) HighestBox = LastBox;
	//LogFile<<"Level2: Gen BPux: "<<LastBox.Position.p[0]<<" BPuy: "<<LastBox.Position.p[1]<<" BPsx: "<<LastBox.Size.p[0]<<" BPzy: "<<LastBox.Size.p[1]<<endl;
	
	//add collision quad and fill vertex/uv buffers
	SetBufferData();
	
	WasHole = false;
	
	//increse current unit position
	CurrentUnit.p[0] += LastBox.Size.p[0];
}

void MLevel2::SetBufferData()
{
	//add vertex data to buffer
	for(int i=0; i<LastBox.Size.p[1]; i++)
	{
		for(int j=0; j<LastBox.Size.p[0]; j++)
		{
			OffsetVertex.push_back(glm::vec2((LastBox.Position.p[0] + j) * UnitSize[0], (LastBox.Position.p[1] + i) * UnitSize[1]));
			//OffsetUV.push_back(glm::vec2(UVUnit[0] * (rand() % SubCount[0]), UVUnit[1] * (rand() % SubCount[1])));
			OffsetUV.push_back(glm::vec2(UVUnit[0] * (rand() % SubCount[0]), UVUnit[1] * LevelTypeOffset));
		}
	}
	
	//merging
	if(BoxNumber && !WasHole)
	{
		if(PrevBox.Position.p[0] + PrevBox.Size.p[0] == LastBox.Position.p[0] && PrevBox.Size.p[1] == LastBox.Size.p[1])
		{
			//add size
			PrevBox.Size.p[0] += LastBox.Size.p[0];
			//delete fixture
			BoxBody->DestroyFixture(Fixtures[BoxNumber - 1]);
			Fixtures.pop_back();
			//recreate fixture
			SetQuad(VertexAdd, PrevBox.Position.p[0] * UnitSize[0] + LevelOffset[0], PrevBox.Position.p[1] * UnitSize[1] + LevelOffset[1],
			PrevBox.Size.p[0] * UnitSize[0], PrevBox.Size.p[1] * UnitSize[1]);
			HalfSize = GetQuadSize(VertexAdd);
			HalfSize *= 0.5;
			Center = GetQuadCenter(VertexAdd);
			Box.SetAsBox(HalfSize.x, HalfSize.y, b2Vec2(Center.x, Center.y), 0.0f);
			Fixtures.push_back(BoxBody->CreateFixture(&Box, FRICTION_SIZE));
			//LogFile<<"Level2: Relocate fixture while merging"<<endl;
					
			return;
		}
	}
	
	//add to world
	SetQuad(VertexAdd, LastBox.Position.p[0] * UnitSize[0] + LevelOffset[0], LastBox.Position.p[1] * UnitSize[1] + LevelOffset[1],
		LastBox.Size.p[0] * UnitSize[0], LastBox.Size.p[1] * UnitSize[1]);
	HalfSize = GetQuadSize(VertexAdd);
	HalfSize *= 0.5;
	Center = GetQuadCenter(VertexAdd);
	Box.SetAsBox(HalfSize.x, HalfSize.y, b2Vec2(Center.x, Center.y), 0.0f);
	Fixtures.push_back(BoxBody->CreateFixture(&Box, FRICTION_SIZE));
	
	BoxNumber ++;
	PrevBox = LastBox;
}

void MLevel2::GenerateHole()
{
	unsigned int RandSize;
	unsigned int LeftSize;
	if(HoleLimit.p[0] != HoleLimit.p[1]) RandSize = (rand() % ((HoleLimit.p[1] - HoleLimit.p[0]) + 1)) + HoleLimit.p[0];
	else RandSize = HoleLimit.p[0];
	LeftSize = CurrentLimit.p[0] - CurrentUnit.p[0];
	if(LeftSize < RandSize) RandSize = LeftSize;
	CurrentUnit.p[0] += RandSize;
	
	//LogFile<<"Level2: Hole size: "<<RandSize<<endl;
	WasHole = true;
}

void MLevel2::GenerateLine()
{
	if(WasConnect)
	{
		//set limits
		CurrentUnit.p[0] = 0;
		CurrentLimit.p[0] = TotalUnit.p[0];
		//HighestBox = LastBox;
		HighestBox.Position.p[0] = 0;
		HighestBox.Position.p[1] = 0;
		HighestBox.Size.p[0] = 0;
		HighestBox.Size.p[1] = 0;
		//select path to generate (left -> last-box or last-box -> right)
		if(LastBox.Position.p[0] > (TotalUnit.p[0] >> 1))
		{
			CurrentUnit.p[0] = 0;
			CurrentLimit.p[0] = LastBox.Position.p[0];
		}
		else
		{
			CurrentUnit.p[0] = LastBox.Position.p[0] + LastBox.Size.p[0];
			CurrentLimit.p[0] = TotalUnit.p[0];
		}
		while(CurrentUnit.p[0] < CurrentLimit.p[0])
		{
			if(!(rand() % 2) && !WasHole) GenerateHole();
			else GenerateBox();
		}
		CurrentUnit.p[1] += HighestBox.Size.p[1];
	}
	else
	{
		GenerateConnect();
		CurrentUnit.p[1] += HighestBox.Size.p[1] + 2;//+1
	}
	WasConnect = !WasConnect;
	//LogFile<<"Level2: Line add "<<WasConnect<<endl;
}

bool MLevel2::GenerateLevel()
{
	//set level offset (not used yet)
	LevelOffset = glm::vec2(0, 0);
	
	//generate
	HighestBox.Position.p[0] = 0;
	HighestBox.Position.p[1] = 0;
	HighestBox.Size.p[0] = 0;
	HighestBox.Size.p[1] = 0;
	LastBox = HighestBox;
	WasConnect = true;
	CurrentUnit.p[0] = 0;
	CurrentUnit.p[1] = 0;
	CurrentLimit.p[0] = TotalUnit.p[0];
	CurrentLimit.p[1] = TotalUnit.p[1];
	BoxNumber = 0;
	//cycle
	while(CurrentUnit.p[1] < CurrentLimit.p[1])
	{
		WasHole = true;
		GenerateLine(); //line may be full or just connect element
		//LogFile<<"Level2: CUy: "<<CurrentUnit.p[1]<<" HighestBox SIZEy: "<<HighestBox.Size.p[1]<<endl;
	}
	//LogFile<<"Level2: Created"<<endl;
	
	return true;
}

bool MLevel2::Create(unsigned int inLevelTypeOffset)
{
	if(!pWorld)
	{
		LogFile<<"Level2: World is NULL"<<endl;
		return false;
	}
	if(!BoxLimit[0].p[0] || !BoxLimit[0].p[1] || !BoxLimit[1].p[0] || !BoxLimit[1].p[1])
	{
		LogFile<<"Level2: NULL box limits"<<endl;
		return false;
	}
	if(BoxLimit[0].p[0] > BoxLimit[0].p[1] || BoxLimit[1].p[0] > BoxLimit[1].p[1])
	{
		LogFile<<"Level2: Wrong box limits"<<endl;
		return false;
	}
	if(!UnitSize[0] || !UnitSize[1])
	{
		LogFile<<"Level2: NULL unit size"<<endl;
		return false;
	}
	if(!TotalUnit.p[0] || !TotalUnit.p[1])
	{
		LogFile<<"Level2: NULL unit count"<<endl;
		return false;
	}
	if(!TextureId || !TexureSize[0] || !TexureSize[1] || !SubCount[0] || !SubCount[1])
	{
		LogFile<<"Level2: Texture data not initialized"<<endl;
		return false;
	}
	if(EdgesOffset.p[0] < 0 || EdgesOffset.p[1] < 0)
	{
		LogFile<<"Level2: Offset less than zero"<<endl;
		return false;
	}
	if(BoxNumber)
	{
		LogFile<<"Level2: Level alredy created"<<endl;
		return false;
	}
	if(inLevelTypeOffset >= SubCount[1])
	{
		LogFile<<"Level2: Level alredy created"<<endl;
		return false;
	}
	LevelTypeOffset = inLevelTypeOffset;
	
	Ready = false;
	
	//physic prepare
	BoxBodyDef.position.Set(0.0f, 0.0f);
	BoxBody = pWorld->CreateBody(&BoxBodyDef);
	
	//LevelTypeOffset = rand() % SubCount[1];
	
	if(!GenerateLevel()) return false;
	
	//create edges
	AddEdge(-EdgesOffset.p[0] - 1, -EdgesOffset.p[1] - 1, 1, TotalUnit.p[1] + EdgesOffset.p[1] * 2 + 1);//left
	AddEdge(TotalUnit.p[0] + EdgesOffset.p[0], -EdgesOffset.p[1] - 1, 1, TotalUnit.p[1] + EdgesOffset.p[1] * 2 + 1);//right
	AddEdge(-EdgesOffset.p[0] - 1, -EdgesOffset.p[1] - 1, TotalUnit.p[0] + EdgesOffset.p[0] + 2, 1);//bottom
	AddEdge(-EdgesOffset.p[0] - 1, TotalUnit.p[1] + EdgesOffset.p[1], TotalUnit.p[0] + EdgesOffset.p[0] + 2, 1);//top
	
	//get vertex array size
	OffsetCount = OffsetVertex.size();
	
	//fill buffers
	glBindBuffer(GL_ARRAY_BUFFER, OffsetVertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * OffsetCount, &OffsetVertex[0][0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, OffsetUVBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * OffsetCount, &OffsetUV[0][0], GL_STATIC_DRAW);
	
	//clear local buffers (level is static)
	OffsetVertex.clear();
	OffsetUV.clear();
	
	Ready = true;
	
	cout<<"Level2: Create: "<<Ready<<endl;
	
	return true;
}

bool MLevel2::CreateTest()
{
	if(!pWorld)
	{
		LogFile<<"Level2: World is NULL"<<endl;
		return false;
	}
	
	Ready = false;
	
	//physic prepare
	BoxBodyDef.position.Set(0.0f, 0.0f);
	BoxBody = pWorld->CreateBody(&BoxBodyDef);
	
	//add graphic and phisic parts of test level
	AddEdge(0, 0, 25, 1);
	AddEdge(0, 0, 1, 18);
	AddEdge(24, 0, 1, 18);
	AddEdge(18, 4, 6, 1);
	
	//get count
	OffsetCount = OffsetVertex.size();
	LogFile<<"Level2: Size: "<<OffsetCount<<endl;
	
	//fill buffers
	glBindBuffer(GL_ARRAY_BUFFER, OffsetVertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * OffsetCount, &OffsetVertex[0][0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, OffsetUVBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * OffsetCount, &OffsetUV[0][0], GL_STATIC_DRAW);
	
	//clear local buffers (level is static)
	OffsetVertex.clear();
	OffsetUV.clear();
	
	Ready = true;
	
	return true;
}

bool MLevel2::Initialize(b2World* inpWorld)
{
	if(!inpWorld)
	{
		LogFile<<"Level2: World is NULL"<<endl;
		return false;
	}
	pWorld = inpWorld;
	BoxBodyDef.position.Set(0.0f, 0.0f);
	BoxBody = pWorld->CreateBody(&BoxBodyDef);
	
	if(!TextureId || !TexureSize[0] || !TexureSize[1] || !SubCount[0] || !SubCount[1])
	{
		LogFile<<"Level2: Texture data not initialized"<<endl;
		return false;
	}
	if(!UnitSize[0] || !UnitSize[1])
	{
		LogFile<<"Level2: NULL UnitSize"<<endl;
		return false;
	}
	
	//calculate min UV unit size
	UVUnit = glm::vec2((float)1 / SubCount[0], (float)1 / SubCount[1]);
	
	//fill static data
	BaseVertex[0] = glm::vec2(0, 0);
	BaseVertex[1] = glm::vec2(0, UnitSize[1]);
	BaseVertex[2] = glm::vec2(UnitSize[0], UnitSize[1]);
	BaseVertex[3] = glm::vec2(UnitSize[0], 0);
	glGenBuffers(1, &BaseVertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, BaseVertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, &BaseVertex[0][0], GL_STATIC_DRAW);
	
	BaseUV[0] = glm::vec2(0, 0);
	BaseUV[1] = glm::vec2(0, UVUnit[1]);
	BaseUV[2] = glm::vec2(UVUnit[0], UVUnit[1]);
	BaseUV[3] = glm::vec2(UVUnit[0], 0);
	glGenBuffers(1, &BaseUVBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, BaseUVBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, &BaseUV[0][0], GL_STATIC_DRAW);
	
	//generate empty buffers
	glGenBuffers(1, &OffsetVertexBufferId);
	glGenBuffers(1, &OffsetUVBufferId);
	
	return true;
}

bool MLevel2::SetTextureData(unsigned int inTextureId, unsigned int Width, unsigned int Height, unsigned int SubCountX, unsigned int SubCountY)
{
	if(!inTextureId)
	{
		LogFile<<"Level2: NULL texture id given"<<endl;
		return false;
	}
	TextureId = inTextureId;
	if(!Width || !Height)
	{
		LogFile<<"Level2: NULL texture width or height"<<endl;
		return false;
	}
	TexureSize[0] = Width;
	TexureSize[1] = Height;
	if(!SubCountX || !SubCountY)
	{
		LogFile<<"Level2: NULL texture sub count"<<endl;
		return false;
	}
	SubCount[0] = SubCountX;
	SubCount[1] = SubCountY;
	
	return true;
}

void MLevel2::Draw()
{
	if(!Ready) return;
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureId);
	
	glBindBuffer(GL_ARRAY_BUFFER, BaseVertexBufferId);
	glVertexAttribPointer(SHR_LAYOUT_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, OffsetVertexBufferId);
	glVertexAttribPointer(SHR_LAYOUT_OFFSET_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, BaseUVBufferId);
	glVertexAttribPointer(SHR_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, OffsetUVBufferId);
	glVertexAttribPointer(SHR_LAYOUT_OFFSET_UV, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glVertexAttribDivisor(SHR_LAYOUT_VERTEX, 0); //basic size one for all
	glVertexAttribDivisor(SHR_LAYOUT_OFFSET_VERTEX, 1); //offset position instance draw
	glVertexAttribDivisor(SHR_LAYOUT_UV, 0); //basic uv one for all
	glVertexAttribDivisor(SHR_LAYOUT_OFFSET_UV, 1); //offset uv instance draw
	glDrawArraysInstanced(GL_QUADS, 0, 4, OffsetCount);
}

bool MLevel2::GetReady()
{
	return Ready;
}

stQuad MLevel2::GetBoxQuadByNumber(unsigned int Number)
{
	stQuad Quad;
	NullQuad(Quad);
	if(!Fixtures.size())
	{
		LogFile<<"Level2: Fixtures array is empty"<<endl;
		return Quad;
	}
	if(Number > Fixtures.size())
	{
		LogFile<<"Level2: Box number out of the fixtures size"<<endl;
		return Quad;
	}
	
	b2PolygonShape* b2PS = (b2PolygonShape*)Fixtures[Number]->GetShape();
	Quad.p[0] = glm::vec2(b2PS->GetVertex(0).x, b2PS->GetVertex(0).y);
	Quad.p[3] = glm::vec2(b2PS->GetVertex(1).x, b2PS->GetVertex(1).y);
	Quad.p[2] = glm::vec2(b2PS->GetVertex(2).x, b2PS->GetVertex(2).y);
	Quad.p[1] = glm::vec2(b2PS->GetVertex(3).x, b2PS->GetVertex(3).y);
	return Quad;
}

void MLevel2::Clear()
{
	//clear offset buffers
	glBindBuffer(GL_ARRAY_BUFFER, OffsetVertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * OffsetCount, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, OffsetUVBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * OffsetCount, NULL, GL_STATIC_DRAW);
	
	//clear fixtures
	if(Fixtures.size() && BoxBody)
	{
		for(int i=0; i<BoxNumber + 4; i++) //4 - edges
			BoxBody->DestroyFixture(Fixtures[i]);
		Fixtures.clear();
	}
	
	//clear box count
	BoxNumber = 0;
}

void MLevel2::Close()
{
	Clear();
	
	//clear static buffers
	glBindBuffer(GL_ARRAY_BUFFER, BaseVertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, BaseUVBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, NULL, GL_STATIC_DRAW);
	
	if(BaseVertexBufferId) glDeleteBuffers(1, &BaseVertexBufferId);
	if(BaseUVBufferId) glDeleteBuffers(1, &BaseUVBufferId);
	if(OffsetVertexBufferId) glDeleteBuffers(1, &OffsetVertexBufferId);
	if(OffsetUVBufferId) glDeleteBuffers(1, &OffsetUVBufferId);
}

bool MLevel2::SetUnit(float UnitSizeX, float UnitSizeY, unsigned int TotalUnitX, unsigned int TotalUnitY)
{
	if(!UnitSizeX || !UnitSizeX)
	{
		LogFile<<"Level2: NULL unit size"<<endl;
		return false;
	}
	if(!TotalUnitX || !TotalUnitY)
	{
		LogFile<<"Level2: NULL total unit count"<<endl;
		return false;
	}
	if(UnitSizeX < 25 || UnitSizeY < 25) LogFile<<"Level2: Warning: too small unit size"<<endl;
	if(TotalUnitX < 25 || TotalUnitY < 18) LogFile<<"Level2: Warning: too few total unit count"<<endl;
	
	UnitSize = glm::vec2(UnitSizeX, UnitSizeY);
	TotalUnit.p[0] = TotalUnitX;
	TotalUnit.p[1] = TotalUnitY;
	
	return true;
}

bool MLevel2::SetBoxLimits(unsigned int UnitLimitX0, unsigned int UnitLimitX1, unsigned int UnitLimitY0, unsigned int UnitLimitY1)
{
	if(!UnitLimitX0 || !UnitLimitX1 || !UnitLimitY0 || !UnitLimitY1)
	{
		LogFile<<"Level2: Some of box limits is NULL"<<endl;
		return false;
	}
	if(UnitLimitX0 < 3) LogFile<<"Level2: Warning: Size box of X is bad (<3)"<<endl;
	
	BoxLimit[0].p[0] = UnitLimitX0;
	BoxLimit[0].p[1] = UnitLimitX1;
	BoxLimit[1].p[0] = UnitLimitY0;
	BoxLimit[1].p[1] = UnitLimitY1;
	
	return true;
}

bool MLevel2::SetHoleLimit(unsigned int UnitLimitX0, unsigned int UnitLimitX1)
{
	HoleLimit.p[0] = UnitLimitX0;
	HoleLimit.p[1] = UnitLimitX1;
	
	return true;
}

bool MLevel2::SetEdgesOffsets(int UnitOffsetX, int UnitOffsetY)
{
	if(UnitOffsetX < 0 || UnitOffsetX < 0)
	{
		LogFile<<"Level2: Offset less than zero"<<endl;
		return false;
	}
	EdgesOffset.p[0] = UnitOffsetX;
	EdgesOffset.p[1] = UnitOffsetY;
	
	return true;
}

glm::vec2 MLevel2::GetUnitSize()
{
	return UnitSize;
}

stQuad MLevel2::GetLastBoxQuad()
{	
	return GetBoxQuadByNumber(Fixtures.size() - 5);// -1(size) - 4(edges)
}

stQuad MLevel2::GetFirstBoxQuad()
{
	return GetBoxQuadByNumber(0);
}

unsigned int MLevel2::GetRandomBoxQuad(stQuad& RetQuad)
{
	unsigned int RndNumber = rand() % (Fixtures.size() - 4);
	RetQuad = GetBoxQuadByNumber(RndNumber);
	return RndNumber;
}

unsigned int MLevel2::GetBoxesCount()
{
	return Fixtures.size() - 4;
}
