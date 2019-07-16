#include "stdafx.h"
#include "Background3.h"

MBackground3::MBackground3()
{
	LinesBufferId = 0;
	pLinesTexture = NULL;
	pLinesTexture_cnt = 0;
	LevelSize = glm::vec2(0, 0);
	LinesCount = 0;
	PartsCount = 0;
	UVShiftSizes[0] = 1;
	UVShiftSizes[1] = 5;
	UVShiftScale = 0.1f;
	StartPoint = glm::vec2(0, 0);
}

MBackground3::~MBackground3()
{
	LinesBufferId = 0;
	pLinesTexture = NULL;
	pLinesTexture_cnt = 0;
	LevelSize = glm::vec2(0, 0);
	LinesCount = 0;
	PartsCount = 0;
}

bool MBackground3::Initialize(stTexture* inpLinesTexture, unsigned int inpLinesTexture_cnt, unsigned int TextureSizeX, unsigned int TextureSizeY, float inScale, float StartX, float StartY)
{
	if(!inpLinesTexture || !inpLinesTexture_cnt)
	{
		LogFile<<"MBackground3: NULL texure"<<endl;
		return false;
	}
	pLinesTexture = inpLinesTexture;
	pLinesTexture_cnt = inpLinesTexture_cnt;
	if(!TextureSizeX || !TextureSizeY)
	{
		LogFile<<"MBackground3: Wrong texure size"<<endl;
		return false;
	}
	TextureSize[0] = TextureSizeX;
	TextureSize[1] = TextureSizeY;
	if(inScale <= 0)
	{
		LogFile<<"MBackground3: Wrong scale"<<endl;
		return false;
	}
	Scale = inScale;
	StartPoint = glm::vec2(StartX, StartY);
	
	return true;
}

bool MBackground3::SetLines(unsigned int inLinesCount, float LevelSizeX, float LevelSizeY, float inLineHeight, unsigned int inPartsCount)
{
	if(!pLinesTexture || !pLinesTexture_cnt)
	{
		LogFile<<"MBackground3: NULL texure"<<endl;
		return false;
	}
	if(!TextureSize[0] || !TextureSize[1])
	{
		LogFile<<"MBackground3: Wrong texure size"<<endl;
		return false;
	}
	if(!inLinesCount)
	{
		LogFile<<"MBackground3: Wrong lines count"<<endl;
		return false;
	}
	LinesCount = inLinesCount;
	if(LevelSizeX <= 0 || LevelSizeY <= 0)
	{
		LogFile<<"MBackground3: Wrong level size"<<endl;
		return false;
	}
	LevelSize = glm::vec2(LevelSizeX, LevelSizeY);
	if(inLineHeight <= 0)
	{
		LogFile<<"MBackground3: Wrong line height"<<endl;
		return false;
	}
	LineHeight = inLineHeight;
	if(inPartsCount < 1)
	{
		LogFile<<"MBackground3: Parts count is NULL"<<endl;
		return false;
	}
	if(pLinesTexture_cnt % inPartsCount != 0 || inPartsCount > pLinesTexture_cnt)
	{
		LogFile<<"MBackground3: Wrong parts count"<<endl;
		return false;
	}
	PartsCount = inPartsCount;
	
	UVSize.x = (float)LevelSize.x * Scale / TextureSize[0];
	UVSize.y = 1;
	LineSize.x = LevelSize.x;
	LineSize.y = (float)TextureSize[1] / Scale;
	
	return true;
}

bool MBackground3::CreateByPartNumber(unsigned int PartNumber)
{
	if(PartNumber > PartsCount || PartsCount < 1)
	{
		LogFile<<"MBackground3: Wrong part number"<<endl;
		return false;
	}
	
	//refill buffer by NULL
	glGenBuffers(1, &LinesBufferId);
	GLenum error = glGetError();
	if(error != GL_NO_ERROR)
	{
		LogFile<<"MBackground3: "<<(char*)gluErrorString(error)<<" "<<error<<endl;
		return false;
	}
	glBindBuffer(GL_ARRAY_BUFFER, LinesBufferId);
	glBufferData(GL_ARRAY_BUFFER, LinesCount * sizeof(stUVQuad), NULL, GL_STATIC_DRAW);
	
	//fill array (random shifts), fill buffer, clear array
	float SubSize = (float)1 / pLinesTexture_cnt;
	if(UVSize.x <= 0 || UVSize.y <= 0)
	{
		LogFile<<"MBackground3: Wrong calculated UV size: "<<UVSize.x<<" "<<UVSize.y<<endl;
		return false;
	}
	stUVQuad AddQuad;
	stQuad UV;
	stQuad Vertex;
	float RandShift;
	unsigned int RandLineNumber;
	vector<stUVQuad> Lines;
	for(unsigned int i=0; i<LinesCount; i++)
	{
		//RandLineNumber = rand() % pLinesTexture_cnt;
		RandLineNumber = rand() % (pLinesTexture_cnt / PartsCount) + PartNumber * (pLinesTexture_cnt / PartsCount);
		RandShift = (rand() % UVShiftSizes[1] + UVShiftSizes[0]) * UVShiftScale;
		SetQuad(Vertex, StartPoint.x + 0.0f, StartPoint.y + i * LineHeight, LineSize.x, LineHeight);
		SetQuad(UV, RandShift, RandLineNumber * SubSize, UVSize.x, SubSize);
		SetUVQuad(AddQuad, Vertex, UV);
		Lines.push_back(AddQuad);
	}
	glBindBuffer(GL_ARRAY_BUFFER, LinesBufferId);
	glBufferData(GL_ARRAY_BUFFER, LinesCount * sizeof(stUVQuad), &Lines[0].p[0][0], GL_STATIC_DRAW);
	Lines.clear();
	
	return true;
}

void MBackground3::Draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pLinesTexture->Id);
	//vertex
	glBindBuffer(GL_ARRAY_BUFFER, LinesBufferId);
	glVertexAttribPointer(SHR_LAYOUT_VERTEX, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	//uv
	glBindBuffer(GL_ARRAY_BUFFER, LinesBufferId);
	glVertexAttribPointer(SHR_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);
	//draw
	glDrawArrays(GL_QUADS, 0, LinesCount * 4);
}

void MBackground3::Close()
{
	glDeleteBuffers(1, &LinesBufferId);
}
