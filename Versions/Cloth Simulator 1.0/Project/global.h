/*******************************************************************
 File: global.h
 Author: Kara Jensen
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments:	game objects/window and function prototypes
			DirectX/Win32 templates from directxtutorial.com and toymaker.info
			ALL GAME FILE HEADERS DON'T INCLUDE THIS
 *******************************************************************/
 

#ifndef GLOBAL_H_
#define GLOBAL_H_

//GAME OBJECTS
#include "semantics.h"
#include "game.h"
#include "gamewindow.h"
#include "camera.h"

//INPUT DATA
struct INPUTDATA
{
    bool Left,Right,Up,Down;
	bool kZ,kX,kC,kV,kA,kS,kW,kQ; //keyboard
	bool k1,k2,k3,k4;
	bool LMB,RMB; //mouse buttons
	int mX,mY; //mouse coordinates
	D3DXVECTOR3 mOrigin,mDir; //mouse ray
	MESH * meshClicked;
	float distanceToMeshClicked;
};

//GLOBAL OBJECTS
extern LPDIRECT3DDEVICE9 d3ddev;
extern LPDIRECT3DSURFACE9 z_buffer;
extern GAMEWINDOW gwd;
extern CAMERA * Camera;
extern INPUTDATA InputData;
extern GAME Game;
extern bool AntiAliasing;

//WINMAIN
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void RegisterWindow(HINSTANCE hInstance);
void DisplayWindow(HINSTANCE hInstance);
bool HandleMessages();
void HandleKeyPress(UINT wParam);

//DIRECTX
void InitDirect3D();
void BuildPresentParams();
void CreateZBuffer();
void SetMultisampling();
void CloseDirect3D();
void StartRender();
void ClearRenderBuffer();
void EndRender();
void InitDirectInput(HINSTANCE hInstance, GAMEWINDOW* gwd);
void GetKeys(BYTE* KeyState);
void GetMouse(DIMOUSESTATE* MouseState);
void CloseDirectInput();

//OTHER
void MainLoop();
void Render();
void Logic();
void Cloth();
void Input();
void ResetInput();
void Picking();

#endif