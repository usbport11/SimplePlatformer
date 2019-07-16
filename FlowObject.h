#ifndef flowobjectH
#define flowobjectH

#include "Object.h"

class MFlowObject: public MObject
{
private:
	glm::vec2 FlowVelocity;
	glm::vec2 SizeVelocity;
	glm::vec2 FlowLimit[2];//flow limit by x(left right) and y(bottom top)
	glm::vec2 SizeLimit;//size limit (right, top)
	glm::vec2 ReturnValue[2];
	glm::vec2 TextureSize;
	
	//timer may be used (if size is increase/descrease by time) or not used
	stTimer Timer;
	static void CALLBACK TimerFunction(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
public:
	MFlowObject();
	~MFlowObject();
	
	bool SetSizeTimer(HWND hWnd, unsigned int TimerValue, bool Enable);
	bool StartSizeTimer();
	void StopSizeTimer();
	bool UpdateSizeTimer(unsigned int TimerValue, bool Enable);
	
	bool SetFlowVelocity(float VelocityX, float VelocityY);
	void SetSizeVelocity(float VelocityX, float VelocityY);
	bool SetTextureSize(float Width, float Height);
	void SetSizeLimit(float SizeLimitX, float SizeLimitY);
	void Flow();
	void Size();
	void Close();
	void Clear();
	
	glm::vec2 GetSize();
};

#endif
