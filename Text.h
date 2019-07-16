#ifndef textH
#define textH

struct stUVBufferIndex
{
	unsigned int Offset;
	unsigned int Size;
};

class MText
{
private:
	float FontSize;
	unsigned int TextureId;
	unsigned int BufferId;
	vector<stUVQuad> Symbol;
	vector<stUVBufferIndex> UVBufferIndex;
	
	stUVQuad UVQuad;
	stQuad Vertex, UV;
	float uv_x, uv_y;
	float Offset;
	float FixX;
	glm::vec2 point_down_left;
	glm::vec2 point_up_left;
	glm::vec2 point_up_right;
	glm::vec2 point_down_right;
	stUVBufferIndex UVBI;
	
public:
	MText();
	~MText();
	bool Initialize(unsigned int inTextureId, float inFontSize, float inFixX);
	bool AddString(float x, float y, char* inString, int Number=-1);
	bool UpdateString(unsigned int Number, float x, float y, char* inString);
	void DeleteString(unsigned int Number);
	void MoveString(unsigned int Number, float x, float y);
	void GetStringRegion(unsigned int Number, glm::vec2& P0, glm::vec2& P1);
	void DrawString(unsigned int Number);
	void DrawAll();
	void Close();
	unsigned int GetStringsCount();
	bool AddStringAndCenterByX(float ResX, float y, char* inString);
	bool UpdateStringAndCenterByX(unsigned int Number, float ResX, float y, char* inString);
};

#endif
