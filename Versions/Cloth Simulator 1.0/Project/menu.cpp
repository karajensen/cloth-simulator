#include "menu.h"
#include "global.h"

//=============================================================
//Load the sprites
//=============================================================
void MENU::OnLoad()
{
	for(int i = 0; i < MAX_BUTTONS; i++);
		buttons.push_back(SPRITE());
	for(int i = 0; i < MAX_BACKDROPS; i++);
		buttons.push_back(SPRITE());
}

//=============================================================
//Draw the menu
//=============================================================
void MENU::OnDraw()
{
}

//=============================================================
//Check if any menu buttons are clicked
//=============================================================
bool MENU::OnClick()
{
	return false;
}