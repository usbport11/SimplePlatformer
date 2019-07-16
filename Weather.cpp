#include "stdafx.h"
#include "Weather.h"
	
MWeather::MWeather()
{
	ParticlesCount = 0;
	CurrentParticlesCount = 4;
	
	VelocityRange[0] = glm::vec2(0, 0);
	VelocityRange[1] = glm::vec2(0, 0);//3,7
	BaseDirection = glm::vec2(-1, -1);
	
	BasePointBufferId = 0;
	OffsetBufferId = 0;
	
	//default zone values
	WindowSize[0] = 800;
	WindowSize[1] = 600;
	ZoneLimit[0] = glm::vec2(0, 800);
	ZoneLimit[1] = glm::vec2(0, 600);
	//x offset not used in this time
	ZoneOffsetSize = glm::vec2(0, 100); //if not null change randomization (0 - ZoneOffsetSize[M]) + ZoneLimit[M][1]
	pCamera = NULL;
	Wind = glm::vec2(0, 0);
}

MWeather::~MWeather()
{
}

bool MWeather::Regenerate(unsigned int Number)
{
	//for movement can be used functions(linear, sin, cos, etc)
	//position
	Offset[Number].x = (rand() % (unsigned int)(CurrentLimit[0][1] - CurrentLimit[0][0]) + CurrentLimit[0][0]) * Scale;
	Offset[Number].y = (rand() % (unsigned int)ZoneOffsetSize[1] + CurrentLimit[1][1]) * Scale;
	//velocity
	Velocity[Number].x = 0;
	Velocity[Number].y = (rand() % (unsigned int)(VelocityRange[1][1] - VelocityRange[1][0]) + VelocityRange[1][0]) * Scale;
	Velocity[Number] *= BaseDirection;
	
	return true;
}

bool MWeather::SetParticles(unsigned int inParticlesCount, float SizeX, float SizeY, float VelocityY0, float VelocityY1, float inScale)
{
	if(!inParticlesCount)
	{
		LogFile<<"Weather: NULL Particles count"<<endl;
		return false;
	}
	if(SizeX <= 0 || SizeY <= 0)
	{
		LogFile<<"Weather: Wrong particle size"<<endl;
		return false;
	}
	if(VelocityY0 <= 0 || VelocityY1 <=0 || VelocityY0 > VelocityY1 || inScale <= 0)
	{
		LogFile<<"Weather: Wrong particle velocity"<<endl;
		return false;
	}

	Scale = inScale;
	ParticlesCount = inParticlesCount;
	
	BasePoint[0] = glm::vec2(0, 0);
	BasePoint[1] = glm::vec2(0, SizeY * Scale);
	BasePoint[2] = glm::vec2(SizeX * Scale, SizeY * Scale);
	BasePoint[3] = glm::vec2(SizeX * Scale, 0);
	
	VelocityRange[1] = glm::vec2(VelocityY0, VelocityY1);
	
	return true;
}

bool MWeather::Initialize()
{
	if(!ParticlesCount)
	{
		LogFile<<"Weather: NULL Particles count"<<endl;
		return false;
	}

	//static data
	glGenBuffers(1, &BasePointBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, BasePointBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, &BasePoint[0][0], GL_STATIC_DRAW);
	
	//dynamic data
	glm::vec2 AddCenter;
	glm::vec2 AddVelocity;
	for(unsigned int i=0; i<ParticlesCount; i++)
	{
		//position
		AddCenter[0] = (rand() % (unsigned int)(CurrentLimit[0][1] - CurrentLimit[0][0]) + CurrentLimit[0][0]) * Scale; 
		AddCenter[1] = (rand() % (unsigned int)ZoneOffsetSize[1] + CurrentLimit[1][1]) * Scale; 
		Offset.push_back(AddCenter);
		//velocity
		AddVelocity[0] = 0;
		AddVelocity[1] = (rand() % (unsigned int)(VelocityRange[1][1] - VelocityRange[1][0]) + VelocityRange[1][0]) * Scale;
		AddVelocity *= BaseDirection;
		Velocity.push_back(AddVelocity);
	}
	glGenBuffers(1, &OffsetBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, OffsetBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * ParticlesCount, NULL, GL_STREAM_DRAW);
	
	return true;
}

bool MWeather::SetZoneParameters(float ZoneLimitX0, float ZoneLimitX1, float ZoneLimitY0, float ZoneLimitY1, float ZoneOffsetSizeX, float ZoneOffsetSizeY, glm::vec3* inpCamera)
{
	//recommend set zone limits with reserve (example window 800x600 - (-100,900,-100,800))
	//it needs to update particles more softly when camera move and zone limits changing
	//don't forget, if you use wind set right zone limits (may create empty angles) 
	//also weather usually going from top to bot (zone offsets for this 0 and 100)
	if(ZoneLimitX0 < 0 || ZoneLimitX1 < 0 || ZoneLimitY0 < 0 || ZoneLimitY1 < 0)
	{
		LogFile<<"Weather: Some of zone limit sub of 0"<<endl;
		return false;
	}
	if(ZoneLimitX0 > ZoneLimitX1 || ZoneLimitY0 > ZoneLimitY1 || ZoneLimitX1 - ZoneLimitX0 < 100 || ZoneLimitY1 - ZoneLimitY0 < 100)
	{
		LogFile<<"Weather: Wrong of zone limit parameters"<<endl;
		return false;
	}
	if(ZoneOffsetSizeX < 0 || ZoneOffsetSizeY < 0)
	{
		LogFile<<"Weather: Some of zone offset sub of 0"<<endl;
		return false;
	}
	if(!ZoneOffsetSizeY)
	{
		LogFile<<"Weather: Zone y offset is NULL. Not supported yet"<<endl;
		return false;
	}
	if(!inpCamera) LogFile<<"Weather: Warning camera not set (locked zone)"<<endl;
	
	ZoneLimit[0] = glm::vec2(ZoneLimitX0, ZoneLimitX1);
	ZoneLimit[1] = glm::vec2(ZoneLimitY0, ZoneLimitY1);
	ZoneOffsetSize = glm::vec2(ZoneOffsetSizeX, ZoneOffsetSizeY);
	pCamera = inpCamera;
	
	//set current Limit
	CurrentLimit[0] = ZoneLimit[0];
	CurrentLimit[1] = ZoneLimit[1];
	
	//trying recalculate zone
	RecalculateZone();
	
	return true;
}

bool MWeather::SetWind(float X, float Y)
{
	if(Y)
	{
		LogFile<<"Weather: Wind by Y not supported"<<endl;
		return false;
	}
	Wind = glm::vec2(X, 0);
	return true;
}

void MWeather::RecalculateZone()
{
	//call when camera pos changed and at start
	if(!pCamera) return;
	CurrentLimit[0][0] = ZoneLimit[0][0] + pCamera->x;
	CurrentLimit[0][1] = ZoneLimit[0][1] + pCamera->x;
	CurrentLimit[1][0] = ZoneLimit[0][0] + pCamera->y;
	CurrentLimit[1][1] = ZoneLimit[0][1] + pCamera->y;
}

void MWeather::MoveParticles()
{
	for(unsigned int i=0; i<CurrentParticlesCount; i++)//ParticlesCount
	{
		Offset[i] += Velocity[i] + Wind;
		if(Offset[i].y < CurrentLimit[1][0] * Scale || (Wind.x && (Offset[i].x < CurrentLimit[0][0] * Scale || Offset[i].x > CurrentLimit[0][1] * Scale)))
		{
			if(CurrentParticlesCount < ParticlesCount) CurrentParticlesCount ++;
			Regenerate(i);
		} 
	}
}

void MWeather::Draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, OffsetBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * ParticlesCount, NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * ParticlesCount, &Offset[0][0]);
	
	glBindBuffer(GL_ARRAY_BUFFER, BasePointBufferId);
	glVertexAttribPointer(SHR_LAYOUT_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, OffsetBufferId);
	glVertexAttribPointer(SHR_LAYOUT_OFFSET_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glVertexAttribDivisor(SHR_LAYOUT_VERTEX, 0); //offset - one by one
	glVertexAttribDivisor(SHR_LAYOUT_OFFSET_VERTEX, 1); //set instance drawing
	glDrawArraysInstanced(GL_QUADS, 0, 4, ParticlesCount);
}

void MWeather::Close()
{
	//clear vectors
	Offset.clear();
	Velocity.clear();
	//close buffers
	glDeleteBuffers(1, &BasePointBufferId);
	glDeleteBuffers(1, &OffsetBufferId);
}
