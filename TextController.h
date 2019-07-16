#ifndef textcontrollerH
#define testcontrollerH

#include "Text.h"

class MTextController
{
private:
	//game strings
	vector<string> GameStrings;
	MText txtGame;
	//hints in load screen (chosed randomly)
	unsigned int HintNumber;
	vector<string> HintsStrings;
	MText txtHints;
	//thanks screen strings
	vector<string> ThanksStrings;
	MText txtThanks;
public:
	MTextController();
	~MTextController();
	bool Initialize(float Width, float Height, unsigned int TextureId);
	bool Size(float Width, float Height);
	void SetRandomHint();
	void DrawHint();
	void DrawGameString(unsigned int Number);
	void DrawThanks();
	void Close();
};

#endif
