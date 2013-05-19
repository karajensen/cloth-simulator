#include "anisprite.h"
#include "global.h" //for inputdata/d3ddev

//=============================================================
//load sprite
//=============================================================
void ANISPRITE::OnLoad(char * filename, int Vis, int Xwidth, int Yheight, float X, float Y, int height, int width, int Columns, int Rows)
{

	vis = Vis; //visibility 0-255 (0=transparent, 255=opaque)
	xWidth = float(Xwidth);
	yHeight = float(Yheight);
    x = X;
	y = Y;
	cols = Columns;
	rows = Rows;
	
	speedCounter = 0;
	frame = 0;
	playing = true;
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
//draw sprite
//=============================================================
void ANISPRITE::OnDraw(float scale)
{
	ScaleFactor = scale;
	float Height = yHeight;
	float Width = xWidth;
	D3DXVECTOR3 position(x, y, 0); //position of sprite
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f); //center at the upper-left corner

	//change width/height according to scale
	if(scale != NULL)
	{	
		Height = Height * ScaleFactor;
		Width = Width * ScaleFactor;
		DrawScaled = true;
	}

	//set framebox
	RECT FrameBox;
    FrameBox.left = (frame % cols) * long(Width);
    FrameBox.top = (frame / rows) * long(Height);
    FrameBox.right = FrameBox.left + long(Width);
    FrameBox.bottom = FrameBox.top + long(Height);

	//DRAW SPRITE
	if(scale != NULL)
	{	
		DrawScaled = true;
		worldMatrix._41 = x;
		worldMatrix._42 = y;
		worldMatrix._43 = 0;
		D3DXMatrixScaling(&worldMatrix,scale,scale,scale);
		d3dspt->SetTransform(&worldMatrix);
		d3dspt->Draw(tex, &FrameBox, &center, &position, D3DCOLOR_ARGB(vis, 255, 255, 255));
		D3DXMatrixIdentity(&worldMatrix);
		d3dspt->SetTransform(&worldMatrix);
	}
	else
	{
		D3DXVECTOR3 position(x, y, 0); //position of sprite
		D3DXVECTOR3 center(0.0f, 0.0f, 0.0f); //center at the upper-left corner
		d3dspt->Draw(tex, &FrameBox, &center, &position, D3DCOLOR_ARGB(vis, 255, 255, 255));
	}
}

//=============================================================
//loop animated sprite; returns false for last frame
//=============================================================
bool ANISPRITE::PlayAnimation(float speed)
{
	if (playing)
	{
		if (speedCounter < SPEED_FACTOR)
			speedCounter += speed;
		else
		{
			//next frame
			int totalFrame = (rows*cols)-1; //conver to 0-based
			speedCounter = 0;
			frame++;
			if (frame == totalFrame) 
				return false;
			else if (frame > totalFrame) //reset if past end
				frame = 1;
		}
	}
	return true;
}