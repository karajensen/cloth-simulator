
#include "gamewindow.h"
#include "global.h" //for reset device

//=============================================================
//constructor
//=============================================================
GAMEWINDOW::GAMEWINDOW(bool win, int w, int h)
{
	hWnd = NULL;
	D3DXMATRIX * matProjection = NULL;
	D3DXMATRIX * matView = NULL;

	saved_screenHeight = h;
	saved_screenWidth = w;

	if (win) //WINDOWED
	{
		Windowed = true;
		WINDOW_WIDTH = w;
		WINDOW_HEIGHT = h;
		win_x_position = (GetSystemMetrics(SM_CXSCREEN)/2)-(WINDOW_WIDTH/2); 
		win_y_position = (GetSystemMetrics(SM_CYSCREEN)/2)-(WINDOW_HEIGHT/2);
	}
	else //FULLSCREEN
	{
		Windowed = false;
		WINDOW_WIDTH = GetSystemMetrics(SM_CXSCREEN);
		WINDOW_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
		win_x_position = 0;
		win_y_position = 0; 
	}
}

