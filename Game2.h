#ifndef game2H
#define game2H

#include "Window.h"
#include "TextureLoader.h"
#include "TextController.h"
#include "MenuController.h"
#include "Stage.h"
#include "SoundSystem.h"

class MGame2;

typedef void (MGame2::*VoidFunc) ();

class MGame2: public MWindow, public MMenuController
{
private:
	VoidFunc pDrawFunc;
	
	//statuses
	bool Pause;
	bool* Key;
	int KeyToChange;
	unsigned char CustomKeys[3];
	
	//classes
	MTextureLoader TextureLoader;
	MTextController TextController;
	MStage Stage;
	MObjectBuffer LocalBuffer;
	MObject* LogoQuad;
	MObject* WaitQuad;
	MObject* LoseQuad;
	
	//sound
	bool SoundEnabled;
	MSoundSystem SoundSystem;
	FMOD_CHANNEL* chnLevel;
	FMOD_CHANNEL* chnOther;
	FMOD_SOUND* sndLevel;
	FMOD_SOUND* sndPickup;
	FMOD_SOUND* sndMenuSelect;
	FMOD_SOUND* sndMenuClick;
	
	//textures
	stTexture* txLogo;
	stTexture* txWait;
	stTexture* txLose;
	stTexture* txMenuPad;
	stTexture* txFont;
	//buffs textures
	stTexture* txBuffs;
	unsigned int txBuffs_cnt;
	//stage textures
	vector<stTexture*> txStage;
	unsigned int OneTexture_cnt;
	
	//shaders data
	GLuint VertexArrayId;
	GLuint ProgramId;
	GLuint MVPId;
	GLuint ColorId;
	GLuint Sampler2DId;
	GLuint UseTextureId;
	GLuint UseOffsetVertexId;
	GLuint UseOffsetUVId;
	
	//matrixes and view
	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Model;
	glm::mat4 MVP;
	glm::mat4 MVPdefault;
	glm::vec3 CameraPosition;
	glm::vec3 CameraDirection;
	float CameraRound;
	float CoordinateScale; //diffrence between resolution and cordinate system (needed for box2d better reaction)
	float ViewScale;
	float ViewOffset;
	glm::vec2 ViewCenter;
	
	//color data
	bool BlinkEnable;
	float ColorLimit[3];
	glm::vec4 CurrentColor[4];
	glm::vec4 LiquidColor[4];
	glm::vec4 HeroColor[4];
	void SetColorLimits(float Max, float Min, float Step);
	void SetCurrentColor(float R, float G, float B, float A);
	void SetColor(glm::vec4* pColor4, float R, float G, float B, float A);
	
	//draw functions
	void DrawGame();
	void DrawWait();
	void DrawLogo();
	void DrawMenu();
	void DrawLose();
	void DrawThanks();
	
	//local
	void Start();
	void Stop();
	
	//window overload functions
	void OnDraw();
	void OnActivate(WPARAM wParam);
	void OnKeyUp(WPARAM wParam, LPARAM lParam);
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnSize();
	void OnMouseHover(WPARAM wParam, LPARAM lParam);
	void OnMouseLButton(WPARAM wParam, LPARAM lParam);
	
	//menu overload functions
	void OnMenuStart();
	void OnMenuThanks();
	void OnMenuExit();
	void OnMenuGraphicsAccept();
	void OnMenuGameplayAccept();
	void OnMenuSoundAccept();
	
	//scale test
	void SetViewScale(float inViewScale);
public:
	MGame2();
	~MGame2();
	bool Initialize(HINSTANCE hInstance);
	void OnClose();
};

#endif
