/*******************************************************************
 File: includes.h
 Author: Kara Jensen                           
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments:	libraries and header files
			ALL GAME FILE HEADERS INCLUDE THIS
 ********************************************************************/

#ifndef INCLUDES_H
#define INCLUDES_H

#define DIRECTINPUT_VERSION 0x0800
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0) //escape key closes program
#define BACKBUFFER_FORMAT (D3DFMT_D16)
#define TEXTURE_FORMAT (D3DFMT_A8R8G8B8)

#define DEGTORAD(degree) ((D3DX_PI / 180.0f) * (degree))
static float TIMESTEP = 0.5f;
static float TIMESTEP_SQUARED = TIMESTEP*TIMESTEP;
static float DAMPING = 0.9f;

//INCLUDES
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include "float3.h"

//STANDARD LIBRARY
#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <fstream>
#include <cmath>
#include <ctime>

//NAMESPACE
using namespace std;

//LIB
#pragma comment (lib, "d3d9.lib") 
#pragma comment (lib, "d3dx9.lib") //Sprites
#pragma comment (lib, "dinput8.lib") //input
#pragma comment (lib, "dxguid.lib") //input

//FORMATTING
const char * ConvertSTR(string & str);

//TEMPLATES
template <class T> void SAFE_RELEASE(T* &p) { if (p) {p->Release(); p = NULL;} };
template <class T> void SAFE_DELETE(T* &p)	{ if (p) {delete p; p = NULL;} };


#endif
