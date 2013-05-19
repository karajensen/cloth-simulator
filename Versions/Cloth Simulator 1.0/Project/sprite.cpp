
#include "global.h" //for inputdata/d3ddev

LPD3DXSPRITE SPRITE::d3dspt = NULL;
D3DXMATRIX SPRITE::worldMatrix;

//=============================================================
//Create the sprite object
//=============================================================
void SPRITE::CreateSpriteObject()
{
	D3DXCreateSprite(d3ddev, &d3dspt);
	D3DXMatrixIdentity(&worldMatrix);
}
//=============================================================
//Toggle the sprites visibility
//=============================================================
void SPRITE::ToggleVisibility(float visOverride)
{
	if(vis < 255)
		vis = 255;
	else
		vis = int(visOverride);
}
//=============================================================
//Release the sprite object
//=============================================================
void SPRITE::ReleaseSpriteObject()
{
	if(d3dspt != NULL)
		d3dspt->Release();
}

//=============================================================
//Destructor
//=============================================================
SPRITE::~SPRITE()
{ 
	if (tex != NULL)
		tex->Release(); 
}

//=============================================================
//Start Overall Sprite Rendering
//=============================================================
void SPRITE::StartSpriteRender()
{
	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);
}

//=============================================================
//Stop Overall Sprite Rendering
//=============================================================
void SPRITE::EndSpriteRender()
{
	d3dspt->End();
}

//=============================================================
//draw sprite
//=============================================================
void SPRITE::OnDraw(float scale)
{
	ScaleFactor = scale;
	D3DXVECTOR3 position(x, y, 0); //position of sprite
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f); //center at the upper-left corner

	if(scale != NULL)
	{	
		DrawScaled = true;
		worldMatrix._41 = x;
		worldMatrix._42 = y;
		worldMatrix._43 = 0;
		D3DXMatrixScaling(&worldMatrix,scale,scale,scale);
		d3dspt->SetTransform(&worldMatrix);
		d3dspt->Draw(tex, NULL, &center, &position, D3DCOLOR_ARGB(vis, 255, 255, 255));
		D3DXMatrixIdentity(&worldMatrix);
		d3dspt->SetTransform(&worldMatrix);
	}
	else
	{
		D3DXVECTOR3 position(x, y, 0); //position of sprite
		D3DXVECTOR3 center(0.0f, 0.0f, 0.0f); //center at the upper-left corner
		d3dspt->Draw(tex, NULL, &center, &position, D3DCOLOR_ARGB(vis, 255, 255, 255));
	}
	
}

//=============================================================
//load sprite
//=============================================================
void SPRITE::OnLoad(char * filename, int Vis, int Xwidth, int Yheight, float X, float Y, int height, int width)
{

	ScaleFactor = 1.0f;
	DrawScaled = false;

	vis = Vis; //visibility 0-255 (0=transparent, 255=opaque)
	xWidth = float(Xwidth);
	yHeight = float(Yheight);
    x = X;
	y = Y;

	visCounter = 0;
  
	if(FAILED(D3DXCreateTextureFromFileEx(d3ddev, filename, width, height, D3DX_DEFAULT, NULL, TEXTURE_FORMAT,
										  D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, NULL, NULL, NULL, &tex)))
	{
		string mess = filename; mess += " failed to load";
		MessageBox(NULL, TEXT(ConvertSTR(mess)), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}
}

//=============================================================
//set sprites position
//=============================================================
void SPRITE::SetNewPos(float xNew, float yNew)
{
	x = xNew;
	y = yNew;
}

//=============================================================
//check mouse click against sprite coordinate
//=============================================================
bool SPRITE::OnClick()
{
	float Height = yHeight;
	float Width = xWidth;

	if(DrawScaled) //if last frame the sprite was scaled
	{
		Height = Height * ScaleFactor;
		Width = Width * ScaleFactor;
		DrawScaled = false;
	}

	if (vis > 1)
	{
		if ((InputData.mY >= y) && (InputData.mY <= (y+Height)) 
			&& (InputData.mX >= x) && (InputData.mX <= (x+Width)))
		{
			return true; //if clicked
		}
	}
	return false; //if not visible or not clicked
}

//=============================================================
//check if visible
//=============================================================
bool SPRITE::IsVisible()
{
	if (vis <= 0)
		return false;
	else
		return true;
}

//=============================================================
//set transparency level
//=============================================================
void SPRITE::SetVisibility(int v)
{
	if (v < 0)
		v = 0;
	else if (v > 255)
		v = 255;

	vis = v;
}

//=============================================================
//fade the sprite
//=============================================================
void SPRITE::FadeVisibility(int speed, int v)
{
	if (vis != v)
	{
		if (v < vis)
		{
			int V = vis - speed;
			SetVisibility(V);
		}
		else
		{
			int V = vis + speed;
			SetVisibility(V);
		}
	}
}


