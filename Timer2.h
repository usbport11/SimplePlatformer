#ifndef timer2H
#define timer2H

#include <sys/time.h>

class MTimer2
{
private:
	unsigned int Id;
	bool Enabled;
	bool Finished;
	timeval EndTime;
	timeval CurrentTime;
	time_t Interval; //in seconds
protected:
	virtual void OnTime();
public:
	bool Create(unsigned int inId, time_t inInterval);
	void Start();
	void Start(time_t inInterval);
	void Stop();
	int Time();
	unsigned int GetId();
	bool GetFinished();
};

#endif
