/********************************************************************
 File: gamewindow.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: class for window
 *******************************************************************/

#ifndef GAMEWINDOW_H_
#define GAMEWINDOW_H_


#include "includes.h"

class GAMEWINDOW
{
public:

	D3DXMATRIX matCurrentProjection;
	D3DXMATRIX matCurrentView; 

	int WINDOW_WIDTH;
	int WINDOW_HEIGHT;
	int win_x_position;
	int win_y_position;
	int saved_screenHeight;
	int saved_screenWidth;
	bool Windowed;

	HWND hWnd;
	GAMEWINDOW(bool win, int w, int h);
};

#endif