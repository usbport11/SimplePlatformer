#ifndef background3H
#define background3H

class MBackground3
{
private:
	GLuint LinesBufferId;
	stTexture* pLinesTexture;
	unsigned int pLinesTexture_cnt;
	unsigned int TextureSize[2];
	unsigned int LinesCount;//total level lines count
	unsigned int PartsCount;//one part - few lines
	unsigned int CurrentPartNumber;
	glm::vec2 LevelSize;
	glm::vec2 UVSize;
	glm::vec2 LineSize;
	glm::vec2 StartPoint;
	float LineHeight;
	float Scale;
	//UV shift by x (for rand)
	unsigned int UVShiftSizes[2];
	float UVShiftScale;
public:
	MBackground3();
	~MBackground3();
	bool Initialize(stTexture* inpLinesTexture, unsigned int inpLinesTexture_cnt, unsigned int TextureSizeX, unsigned int TextureSizeY, float inScale, float StartX, float StartY);
	bool SetLines(unsigned int inLinesCount, float LevelSizeX, float LevelSizeY, float inLineHeight, unsigned int inPartsCount);
	bool CreateByPartNumber(unsigned int PartNumber);
	void Draw();
	void Close();
};

#endif
