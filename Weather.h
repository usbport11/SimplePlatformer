#ifndef weatherH
#define weatherH

class MWeather
{
private:
	unsigned int ParticlesCount;
	unsigned int CurrentParticlesCount;
	
	glm::vec2 BasePoint[4]; //base point size
	glm::vec4 BaseColor[4]; //point color
	vector<glm::vec2> Offset;
	glm::vec2 VelocityRange[2];
	glm::vec2 BaseDirection;
	vector<glm::vec2> Velocity;
	float Scale;
	
	GLuint BasePointBufferId;
	GLuint OffsetBufferId;
	
	unsigned int WindowSize[2]; //?
	glm::vec2 ZoneLimit[2]; //0-x, 1-y
	glm::vec2 CurrentLimit[2];
	glm::vec2 ZoneOffsetSize; //zone offset size;
	glm::vec3* pCamera; //camera data for flow zone
	glm::vec2 Wind;
	
	bool Regenerate(unsigned int Number);
	
public:
	MWeather();
	~MWeather();

	bool Initialize();
	bool SetParticles(unsigned int inParticlesCount, float SizeX, float SizeY, float VelocityY0, float VelocityY1, float inScale);
	bool SetZoneParameters(float ZoneLimitX0, float ZoneLimitX1, float ZoneLimitY0, float ZoneLimitY1, float ZoneOffsetSizeX, float ZoneOffsetSizeY, glm::vec3* inpCamera);
	bool SetWind(float X, float Y);
	void RecalculateZone();
	void MoveParticles();
	void Draw();
	void Close();
};

#endif
