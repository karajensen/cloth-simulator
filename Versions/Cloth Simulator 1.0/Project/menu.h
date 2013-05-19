/********************************************************************
 File: menu.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: Game menu class
 *******************************************************************/

#ifndef MENU_H_
#define MENU_H_

#include "includes.h"
#include "sprite.h"

class MENU
{
private:

	enum
	{
		MENU1 = 0
	};

	static const int MAX_BUTTONS = 0;
	static const int MAX_BACKDROPS = 0;
	vector<SPRITE> buttons;
	vector<SPRITE> backdrops;

public:
	bool OnClick();
	void OnLoad();
	void OnDraw();
};

#endif


