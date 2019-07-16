#include "stdafx.h"
#include "Interface.h"

MInterface::MInterface()
{
	BuffsStart = glm::vec2(0.32f, 0.5f);
	BuffsSize = glm::vec2(0.32f, 0.32f);
	ActiveBuffsCount = 0;
	ptxBuffs = NULL;
	BottomPlane = NULL;
	ptxBottomPlane = NULL;
	BottomPlaneSize = glm::vec2(6.0f, 0.5f);
	pLevelSize = NULL;
}

MInterface::~MInterface()
{
	ActiveBuffsCount = 0;
	ptxBuffs = NULL;
	BottomPlane = NULL;
	ptxBottomPlane = NULL;
	pLevelSize = NULL;
}

bool MInterface::Initialize(stTexture* inptxBuffs, unsigned int BuffsCount, stTexture* inptxBottomPlane, glm::vec2* inpLevelSize)
{
	if(!inpLevelSize)
	{
		LogFile<<"Interface: NULL level size"<<endl;
		return false;
	}
	if(inpLevelSize->x < 7)
	{
		LogFile<<"Interface: Wrong level size: "<<inpLevelSize->x<<" "<<inpLevelSize->y<<endl;
		return false;
	}
	pLevelSize = inpLevelSize;
	if(!inptxBuffs || !inptxBottomPlane)
	{
		LogFile<<"Interface: NULL textures array"<<endl;
		return false;
	}
	if(!BuffsCount)
	{
		LogFile<<"Interface: Wrong Buffs count"<<endl;
		return false;
	}
	ptxBuffs = inptxBuffs;
	ptxBottomPlane = inptxBottomPlane;
	try
	{
		for(unsigned int i=0; i<BuffsCount; i++)
			Buffs.push_back(new MObject);
	}
	catch(bad_alloc& ba)
	{
		LogFile<<"Interface: Can't allocate memory for Buffs"<<endl;
		return false;
	}
	//create empty buffs buffer
	if(!BuffsBuffer.Initialize(GL_STREAM_DRAW)) return false;
	BuffsBuffer.DisposeAll();
	//create ui elements
	BottomPlane = new MObject;
	if(!UIElements.Initialize(GL_STREAM_DRAW)) return false;
	if(!UIElements.AddObject(BottomPlane, 1.0f, 0.0f, 6.0f, 0.5f, 0,0,1,1, ptxBottomPlane->Id)) return false;
	UIElements.DisposeAll();
	
	return true;
}

int MInterface::AddBuff(unsigned int TypeNumber)
{
	if(TypeNumber >= Buffs.size())
	{
		LogFile<<"Interface: Wrong type number (too big): "<<TypeNumber<<" "<<Buffs.size()<<endl;
		return ADB_ERROR;
	}
	it = NumberMap.find(TypeNumber);
	if(it != NumberMap.end()) 
	{
		cout<<"Buff alredy existed!"<<endl;
		return ADB_EXIST;
	}
	//add to video buffer
	if(!BuffsBuffer.AddObject(Buffs[TypeNumber], BuffsStart.x + BuffsSize.x * ActiveBuffsCount, BuffsStart.y, BuffsSize.x, BuffsSize.y, 0,0,1,1, ptxBuffs[TypeNumber].Id))
	{
		LogFile<<"Interface: Can't add buff in buff buffer"<<endl;
		return ADB_ERROR;
	}
	NumberMap.insert(pair<unsigned int, MObject*>(TypeNumber, Buffs[TypeNumber]));
	ActiveBuffsCount ++;
	cout<<"Add buff to buffer"<<endl;
	if(!BuffsBuffer.DisposeAll()) return ADB_ERROR;//IMOPTANT!
	
	return ADB_OK;
}

bool MInterface::RemoveBuff(unsigned int TypeNumber)
{
	it = NumberMap.find(TypeNumber);
	if(it == NumberMap.end())
	{
		LogFile<<"Interface: Can't find buff in map"<<endl;
		return false;
	}
	NumberMap.erase(it);	
	if(!BuffsBuffer.RemoveObject(Buffs[TypeNumber]))
	{
		LogFile<<"Interface: Can't remove buff from buffer"<<endl;
		return false;
	}
	if(ActiveBuffsCount) ActiveBuffsCount --;
	stQuad Vertex;
	unsigned int i=0;
	for(it = NumberMap.begin(); it != NumberMap.end(); ++it)
	{
		SetQuad(Vertex, BuffsStart.x + i * BuffsSize.x, BuffsStart.y, BuffsSize.x, BuffsSize.y);
		it->second->SetVertex(Vertex);
		i++;
	}
	BuffsBuffer.UpdateAll();
	cout<<"i: "<<i<<endl;
	cout<<"Remove buff from buffer"<<endl;
	
	return true;
}

void MInterface::Draw()
{
	UIElements.DrawAll();
	BuffsBuffer.DrawAll();
	DrawStats();
}

void MInterface::Clear()
{
	ActiveBuffsCount = 0;
	BuffsBuffer.RemoveAll();
	NumberMap.clear();
}

void MInterface::Close()
{
	Clear();
	BuffsBuffer.Close();
	
	UIElements.Close();
	if(BottomPlane) delete BottomPlane;
	
	for(unsigned int i=0; i<Buffs.size(); i++)
	{
		if(Buffs[i]) delete Buffs[i];
	}
	Buffs.clear();
	NumberMap.clear();
	
	LevelStatsCaptions.clear();
	txtLevelStatsCaptions.Close();
	txtLevelStatsValues.Close();
}

void MInterface::Size(float Width, float Height)
{
	if(!BottomPlane) return;
	//bottom plane resize
	stQuad Vertex;
	//SetQuad(Vertex, (Width - BottomPlaneSize.x) * 0.5f, 0.0f, BottomPlaneSize.x, BottomPlaneSize.y);
	SetQuad(Vertex, (Width - pLevelSize->x) * 0.5f, 0.0f, pLevelSize->x, BottomPlaneSize.y);
	BottomPlane->SetVertex(Vertex);
	UIElements.UpdateObject(BottomPlane);
	//buff offset
	BuffsStart = glm::vec2(Vertex.p[0].x + 0.96f, Vertex.p[0].y + 0.08f);
	
	//stats size
	for(unsigned int i=0; i<LevelStatsCaptions.size(); i++)
	{
		if(!txtLevelStatsCaptions.UpdateString(i, 0, Height - 0.5 - i * 0.2, (char*)LevelStatsCaptions[i].c_str())) return;
		LevelStatsCoords[i] = glm::vec2(LevelStatsCaptions[i].length() * 0.09f, Height - 0.5 - i * 0.2);//0.65
		if(!txtLevelStatsValues.UpdateString(i, LevelStatsCoords.back().x, LevelStatsCoords.back().y, (char*)"1")) return;
	}
}

bool MInterface::InitializeStats(unsigned int TextureId, float SizeX, float SizeY, float Width, float Height)
{
	if(!TextureId) return false;

	//level statisics
	LevelStatsCaptions.push_back("Level: ");//will be more stats values
	LevelStatsCaptions.push_back("Exit: ");
	LevelStatsCaptions.push_back("Fill: ");
	
	if(!txtLevelStatsCaptions.Initialize(TextureId, SizeX, SizeY)) return false;
	if(!txtLevelStatsValues.Initialize(TextureId, SizeX, SizeY)) return false;
	for(unsigned int i=0; i<LevelStatsCaptions.size(); i++)
	{
		if(!txtLevelStatsCaptions.AddString(0, Height - 0.5 - i * 0.2, (char*)LevelStatsCaptions[i].c_str())) return false;
		LevelStatsCoords.push_back(glm::vec2(LevelStatsCaptions[i].length() * 0.09f, Height - 0.5 - i * 0.2));//0.65
		if(!txtLevelStatsValues.AddString(LevelStatsCoords.back().x, LevelStatsCoords.back().y, (char*)"1")) return false;
	}
	
	return true;
}

bool MInterface::SetStatValue(unsigned int Number, unsigned int Value)
{
	memset(ConvertBuffer, 0, 5);
	return txtLevelStatsValues.UpdateString(Number, LevelStatsCoords[Number].x, LevelStatsCoords[Number].y, itoa(Value, ConvertBuffer, 10));
}

bool MInterface::SetStatValue(unsigned int Number, float Value)
{
	memset(ConvertBuffer, 0, 5);
	sprintf(ConvertBuffer, "%3.2f", Value);
	return txtLevelStatsValues.UpdateString(Number, LevelStatsCoords[Number].x, LevelStatsCoords[Number].y, ConvertBuffer);
}

void MInterface::DrawStats()
{
	txtLevelStatsCaptions.DrawAll();
	txtLevelStatsValues.DrawAll();
}
