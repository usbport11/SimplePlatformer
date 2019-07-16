#include "stdafx.h"
#include "Text.h"

MText::MText()
{
	TextureId = 0;
	BufferId = 0;
	Offset = (float)1 / 16;
	FixX = 0.14;
}

MText::~MText()
{
}

bool MText::Initialize(unsigned int inTextureId, float inFontSize, float inFixX)
{
	if(inFontSize < 0 || inFontSize > 32)
	{
		LogFile<<"Text: Font height too small or too big"<<endl;
		return false;
	}
	if(!inTextureId)
	{
		LogFile<<"Text: NULL font texture"<<endl;
		return false;
	}
	
	FontSize = inFontSize;
	TextureId = inTextureId;
	FixX = inFixX;
	
	//gen buffer
	glGenBuffers(1, &BufferId);
	GLenum error = glGetError();
	if( error != GL_NO_ERROR )
    {
		LogFile<<"Text: "<<(char*)gluErrorString( error )<<" "<<error<<endl;
		return false;
    }
	
	return true;
}

void MText::Close()
{
	Symbol.clear();
	UVBufferIndex.clear();
	if(BufferId) glDeleteBuffers(1, &BufferId);
	LogFile<<"Text: Buffer closed. Id: "<<BufferId<<endl;
}

bool MText::AddString(float x, float y, char* inString, int Number)
{
	unsigned int Length = strlen(inString);
	if(!Length)
	{
		LogFile<<"Text: Empty string given"<<endl;
		return false;
	}
	if(!BufferId)
	{
		LogFile<<"Text: NULL buffer "<<endl;
		return false;
	}

	if(Number < 0) UVBI.Offset = Symbol.size();
	else UVBI.Offset = UVBufferIndex[Number].Offset;
	UVBI.Size = Length;
	for(unsigned int i=0; i<Length; i++)
	{
		point_down_left = glm::vec2(x + i * FontSize - i * FixX, y);
		point_up_left = glm::vec2(x + i * FontSize - i * FixX, y + FontSize);
		point_up_right = glm::vec2(x + i * FontSize + FontSize - i * FixX, y + FontSize);
		point_down_right = glm::vec2(x + i * FontSize + FontSize - i * FixX, y);
		SetQuad(Vertex, point_down_left, point_up_left, point_up_right, point_down_right);
	
		//16 - count symbols in row
		//32 - size of one symbol in pixels
		//512 - size of texture
		//subtruct 1 - swap top and bot UV coordinates
		uv_x = (float)(inString[i] % 16) / 16;
		uv_y = (float)(1 - (float)(inString[i] / 16) / 16 + (float)32 / 512);
		
		point_down_left = glm::vec2(uv_x, uv_y);
		point_up_left = glm::vec2(uv_x, uv_y + Offset);
		point_up_right = glm::vec2(uv_x + Offset, uv_y + Offset);
		point_down_right = glm::vec2(uv_x + Offset, uv_y);
		SetQuad(UV, point_down_left, point_up_left, point_up_right, point_down_right);
		
		SetUVQuad(UVQuad, Vertex, UV);
		if(Number < 0) Symbol.push_back(UVQuad);
		else Symbol.insert(Symbol.begin() + UVBI.Offset + i, UVQuad);
	}
	if(Number < 0) UVBufferIndex.push_back(UVBI);
	else
	{
		//IMPORTANT!
		int DiffSize = UVBI.Size - UVBufferIndex[Number].Size;
		UVBufferIndex[Number] = UVBI;
		for(unsigned int i=Number + 1; i<UVBufferIndex.size(); i++)
			UVBufferIndex[i].Offset += DiffSize;
	} 
	
	//relocate all buffer
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glBufferData(GL_ARRAY_BUFFER, Symbol.size() * sizeof(stUVQuad), &Symbol[0].p[0][0].x, GL_STATIC_DRAW);
	
	return true;
}

bool MText::UpdateString(unsigned int Number, float x, float y, char* inString)
{
	if(!BufferId || !Symbol.size() || Number >= UVBufferIndex.size()) return false;
	//erase UV
	Symbol.erase(Symbol.begin() + UVBufferIndex[Number].Offset, Symbol.begin() + UVBufferIndex[Number].Offset + UVBufferIndex[Number].Size);
	//update
	if(!AddString(x, y, inString, Number)) return false;
	
	return true;
}

void MText::DeleteString(unsigned int Number)
{
	if(!BufferId || !Symbol.size() || Number >= UVBufferIndex.size()) return;
	//erase UV
	Symbol.erase(Symbol.begin() + UVBufferIndex[Number].Offset, Symbol.begin() + UVBufferIndex[Number].Offset + UVBufferIndex[Number].Size);
	//erase string
	UVBufferIndex.erase(UVBufferIndex.begin() + Number);
	//relocate all buffer
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glBufferData(GL_ARRAY_BUFFER, Symbol.size() * sizeof(stUVQuad), &Symbol[0].p[0][0].x, GL_STATIC_DRAW);
}

void MText::MoveString(unsigned int Number, float x, float y)
{
	if(!BufferId || !Symbol.size() || Number >= UVBufferIndex.size()) return;
	glm::vec2 Diff = glm::vec2(x - Symbol[UVBufferIndex[Number].Offset].p[0][0].x, y - Symbol[UVBufferIndex[Number].Offset].p[0][0].y);
	for(unsigned int i=UVBufferIndex[Number].Offset; i<UVBufferIndex[Number].Offset + UVBufferIndex[Number].Size; i++)
	{
		MoveUVQuad(Symbol[i], Diff.x, Diff.y);
	}
}

void MText::GetStringRegion(unsigned int Number, glm::vec2& P0, glm::vec2& P1)
{
	if(!BufferId || !Symbol.size() || Number >= UVBufferIndex.size()) return;
	P0 = Symbol[UVBufferIndex[Number].Offset].p[0][0];
	P1 = Symbol[UVBufferIndex[Number].Offset + UVBufferIndex[Number].Size - 1].p[2][0];
}

void MText::DrawString(unsigned int Number)
{
	if(!BufferId || !Symbol.size() || Number >= UVBufferIndex.size()) return;
	//texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureId);
	//vertex
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glVertexAttribPointer(SHR_LAYOUT_VERTEX, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	//uv
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glVertexAttribPointer(SHR_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	//draw
	glDrawArrays(GL_QUADS, UVBufferIndex[Number].Offset * 4, UVBufferIndex[Number].Size * 4);
}

void MText::DrawAll()
{
	if(!BufferId || !Symbol.size()) return;
	//texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureId);
	//vertex
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glVertexAttribPointer(SHR_LAYOUT_VERTEX, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	//uv
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glVertexAttribPointer(SHR_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);
	//draw
	glDrawArrays(GL_QUADS, 0, Symbol.size() * 4);
}

unsigned int MText::GetStringsCount()
{
	return UVBufferIndex.size();
}

bool MText::AddStringAndCenterByX(float ResX, float y, char* inString)
{
	float StartX =  ResX * 0.5 - strlen(inString) * (FontSize - FixX) * 0.5;
	return AddString(StartX, y, (char*)inString);
}

bool MText::UpdateStringAndCenterByX(unsigned int Number, float ResX, float y, char* inString)
{
	float StartX =  ResX * 0.5 - strlen(inString) * (FontSize - FixX) * 0.5;
	return UpdateString(Number, StartX, y, (char*)inString);
}

