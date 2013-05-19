
///REMOVE THIS ONCE GUI UPDATED///

#pragma once
#include "common.h"

class Picking;

class Sprite
{
public:

	static LPD3DXSPRITE d3dspt;
	static void CreateSpriteObject(LPDIRECT3DDEVICE9 d3ddev);
	static void StartSpriteRender();
	static void EndSpriteRender();
	static void ReleaseSpriteObject();
	static void SetWorldMatrix(D3DXMATRIX & world){worldMatrix = world;};

	Sprite(){ tex = NULL; };
	~Sprite();
	void OnDraw(float scale = NULL);
	bool OnClick(int clickX, int clickY);
	void Event(){};
	bool Load(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename, int Vis, int Xwidth, int Yheight, float X, float Y, int height, int width);
	bool IsVisible();
	void SetVisibility(int v);
	int GetVisibility(){return vis;};
	void ToggleVisibility(float visOverride = 127.5);
	void FadeVisibility(int speed, int v);
	void SetNewPos(float xNew, float yNew);
	LPDIRECT3DTEXTURE9 & GetImage(){ return tex; };

private:

	int vis;
	float xWidth, yHeight; //frame width/height
	float x, y; //top left position
	LPDIRECT3DTEXTURE9 tex;
	float visCounter;
	static D3DXMATRIX worldMatrix;

	bool DrawScaled;
	float ScaleFactor;
};
