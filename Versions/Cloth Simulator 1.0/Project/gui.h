/********************************************************************
 File: GUI.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: User interface
 *******************************************************************/

#ifndef GUI_H_
#define GUI_H_

#include "includes.h"
#include "sprite.h"
#include "anisprite.h"
#include "text.h"
#include "clock.h"

class GUI
{
private:

	static const int MAX_GUIANISPRITES = 0;
	static const int MAX_GUISPRITES = 0;
	static const int MAX_GUITEXT = 0;

	//vector<SPRITE> guiSprites;
	//vector<ANISPRITE> guiAnisprites;
	//vector<DTEXT> guiText;
	CLOCK clock;

public:
	void OnLoad();
	void OnDrawGUISprites();
	void OnDrawGUIText();
	bool OnClickGUI();
	void OnLostDevice();
	void OnResetDevice();

	void AnimateGUI();
	void SetText(string t, int index);
	void SetText(char * t, int index);

	void StartClocks();
	void StopClocks();
	void CalculateClocks();
	CLOCK * GetClock(){return &clock;};
};

#endif


