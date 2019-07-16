#ifndef soundsystemH
#define soundsystemH

#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>

class MSoundSystem
{
private:
	FMOD_SYSTEM* m_pSystem;
	FMOD_RESULT Res;
	FMOD_BOOL Playing;
	int driverCount;
	float Volume;
public:
	MSoundSystem();
	bool Initialize();	
	bool CreateSound(FMOD_SOUND** pSound, const char* pFile);
	bool PlaySound(FMOD_SOUND* pSound, FMOD_CHANNEL** pChannel, bool bLoop = false);
	bool StopPlay(FMOD_CHANNEL* pChannel);
	bool IsPlaying(FMOD_CHANNEL* pChannel);
	bool SetVolume(float inVolume);
	void ReleaseSound(FMOD_SOUND* pSound);
	void Close();
};

#endif
