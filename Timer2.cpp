#include "Timer2.h"

void MTimer2::OnTime()
{
}

bool MTimer2::Create(unsigned int inId, time_t inInterval)
{
	if(!inId) return false;
	if(inInterval < 0) return false; 
	Id = inId;
	Interval = inInterval;
	Enabled = false;
	Finished = false;
}

void MTimer2::Start()
{
	Enabled = true;
	Finished = false;
	gettimeofday(&EndTime, NULL);
	EndTime.tv_sec += Interval;
}

void MTimer2::Start(time_t inInterval)
{
	if(inInterval <= 0) return;
	Interval = inInterval;
	Enabled = true;
	Finished = false;
	gettimeofday(&EndTime, NULL);
	EndTime.tv_sec += Interval;
}

void MTimer2::Stop()
{
	Enabled = false;
}

int MTimer2::Time()
{
	if(!Enabled) return 0;
	gettimeofday(&CurrentTime, NULL);
	if(CurrentTime.tv_sec >= EndTime.tv_sec)
	{
		OnTime();
		Stop();
		Finished = true;
		return 1;
	}
    else return 0;
}

unsigned int MTimer2::GetId()
{
	return Id;
}

bool MTimer2::GetFinished()
{
	return Finished;
}
