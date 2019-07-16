#include "stdafx.h"
#include "SoundSystem.h"

MSoundSystem::MSoundSystem()
{
	m_pSystem = NULL;
	driverCount = 0;
	Volume = 1.0f; //max
}

bool MSoundSystem::Initialize()
{
	Res = FMOD_System_Create(&m_pSystem);
	if(Res != FMOD_OK)
	{
		LogFile<<"Can not initialize fmod: "<<FMOD_ErrorString(Res)<<endl;
		return false;
	}
	Res = FMOD_System_GetNumDrivers(m_pSystem, &driverCount);
	if(Res != FMOD_OK)
	{
		LogFile<<"Can not get drv count: "<<FMOD_ErrorString(Res)<<endl;
		return false;
	}
	if(!driverCount)
	{
		LogFile<<"No sound devices"<<endl;
		return false;
	}
	Res = FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);
	if(Res != FMOD_OK)
	{
		LogFile<<"Can not initialize sound system: "<<FMOD_ErrorString(Res)<<endl;
		return false;
	}
	return true;
}

bool MSoundSystem::CreateSound(FMOD_SOUND** pSound, const char* pFile)
{
	Res = FMOD_System_CreateSound(m_pSystem, pFile, FMOD_CREATESTREAM, 0, pSound);//FMOD_DEFAULT 
	if(Res != FMOD_OK)
	{
		LogFile<<"Can not create sound: "<<FMOD_ErrorString(Res)<<endl;
		return false;
	}
	return true;
}

bool MSoundSystem::PlaySound(FMOD_SOUND* pSound, FMOD_CHANNEL** pChannel, bool bLoop)
{
	if(!bLoop)
	{
		Res = FMOD_Sound_SetMode(pSound, FMOD_LOOP_OFF);
		if(Res != FMOD_OK)
		{
			LogFile<<"Can not set sound mode NOLOOP: "<<FMOD_ErrorString(Res)<<endl;
			return false;
		}
	}
	else
	{
		Res = FMOD_Sound_SetMode(pSound, FMOD_LOOP_NORMAL);
		if(Res != FMOD_OK)
		{
			LogFile<<"Can not set sound mode LOOP: "<<FMOD_ErrorString(Res)<<endl;
			return false;
		}
		FMOD_Sound_SetLoopCount(pSound, -1);
	}
	Res = FMOD_System_PlaySound(m_pSystem, pSound, 0, false, pChannel);
	if(Res != FMOD_OK)
	{
		LogFile<<"Can not play sound: "<<FMOD_ErrorString(Res)<<endl;
		return false;
	}
	Res = FMOD_Channel_SetVolume(*pChannel, Volume);
	if(Res != FMOD_OK)
	{
		LogFile<<"Can not set volume: "<<FMOD_ErrorString(Res)<<endl;
		return false;
	}
	Playing = true;
	return true;
}

bool MSoundSystem::StopPlay(FMOD_CHANNEL* pChannel)
{
	Res = FMOD_Channel_Stop(pChannel);
	if(Res != FMOD_OK)
	{
		LogFile<<"Can not stop: "<<FMOD_ErrorString(Res)<<endl;
		return false;
	}
	return true;
}

bool MSoundSystem::IsPlaying(FMOD_CHANNEL* pChannel)
{
	if(pChannel)
	{
		Res = FMOD_Channel_IsPlaying(pChannel, &Playing);
		if(Res != FMOD_OK)
		{
			LogFile<<"Can not get play status: "<<FMOD_ErrorString(Res)<<endl;
			return false;
		}
		return Playing;
	}
	return false;
}

bool MSoundSystem::SetVolume(float inVolume)
{
	Volume = inVolume;
	return true;
}

void MSoundSystem::ReleaseSound(FMOD_SOUND* pSound)
{
	FMOD_Sound_Release(pSound);
}

void MSoundSystem::Close()
{
	Res = FMOD_System_Release(m_pSystem);
	if(Res != FMOD_OK) LogFile<<"Can not release sound system: "<<FMOD_ErrorString(Res)<<endl;
}
