/********************************************************************
 File: sprite.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: 2D Sprite class sharing the one sprite object
 *******************************************************************/

#ifndef SPRITE_H_
#define SPRITE_H_


#include "includes.h"

class SPRITE
{

protected:
	int vis;
	float xWidth, yHeight; //frame width/height
	float x, y; //top left position
	LPDIRECT3DTEXTURE9 tex;
	float visCounter;
	static D3DXMATRIX worldMatrix;

	bool DrawScaled;
	float ScaleFactor;

public:

	//shared sprite object
	static LPD3DXSPRITE d3dspt;
	static void CreateSpriteObject();
	static void StartSpriteRender();
	static void EndSpriteRender();
	static void ReleaseSpriteObject();
	static void SetWorldMatrix(D3DXMATRIX & world){worldMatrix = world;};

	SPRITE(){ tex = NULL; };
	~SPRITE();
	void OnDraw(float scale = NULL);
	bool OnClick();
	void Event(){};
	void OnLoad(char * filename, int Vis, int Xwidth, int Yheight, float X, float Y, int height, int width);
	bool IsVisible();
	void SetVisibility(int v);
	int GetVisibility(){return vis;};
	void ToggleVisibility(float visOverride = 127.5);
	void FadeVisibility(int speed, int v);
	void SetNewPos(float xNew, float yNew);
	LPDIRECT3DTEXTURE9 & GetImage(){ return tex; };


};

#endif


