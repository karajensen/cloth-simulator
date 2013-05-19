///////////////////////////////////////////////////////////////////////////////////
//GAME INPUT (this translates DirectInput into the InputData struct)
///////////////////////////////////////////////////////////////////////////////////


#include "includes.h"
#include "global.h"

DIMOUSESTATE Mouse;
BYTE Keys[256];

//=============================================================
//GAME INPUT
//=============================================================
void Input()
{
	GetMouse(&Mouse);
    GetKeys(&Keys[0]);

	//KEYBOARD INPUT
    if(Keys[DIK_UP])
        InputData.Up = true;
    if(Keys[DIK_DOWN])
        InputData.Down = true;
    if(Keys[DIK_LEFT])
        InputData.Left = true;
    if(Keys[DIK_RIGHT])
        InputData.Right = true;
	if(Keys[DIK_Z])
		InputData.kZ = true;
	if(Keys[DIK_X])
		InputData.kX = true;
	if(Keys[DIK_C])
		InputData.kC = true;
	if(Keys[DIK_V])
		InputData.kV = true;
	if(Keys[DIK_A])
		InputData.kA = true;
	if(Keys[DIK_S])
		InputData.kS = true;
	if(Keys[DIK_Q])
		InputData.kQ = true;
	if(Keys[DIK_W])
		InputData.kW = true;
	if(Keys[DIK_1])
		InputData.k1 = true;
	if(Keys[DIK_2])
		InputData.k2 = true;
	if(Keys[DIK_3])
		InputData.k3 = true;
	if(Keys[DIK_4])
		InputData.k4 = true;

	//MOUSE INPUT
	if(Mouse.rgbButtons[0]) //LEFT MB
		InputData.LMB = true;
	if(Mouse.rgbButtons[1]) //RIGHT MB
		InputData.RMB = true;

}

void Picking()
{
	//REFERENCE: http://www.toymaker.info/Games/html/picking.html
	D3DXVECTOR3 mouseRay;
	mouseRay.x =  ( ( ( 2.0f * InputData.mX ) / gwd.WINDOW_WIDTH  ) - 1 ) / Camera->matProjection._11;
	mouseRay.y = -( ( ( 2.0f * InputData.mY ) / gwd.WINDOW_HEIGHT ) - 1 ) / Camera->matProjection._22;
	mouseRay.z =  1.0f;

	D3DXMATRIX ViewInverse;
	D3DXMatrixInverse(&ViewInverse, NULL, &Camera->matView);

	//Transform the screen space pick ray into 3D world space
	InputData.mDir.x  = mouseRay.x*ViewInverse._11 + mouseRay.y*ViewInverse._21 + mouseRay.z*ViewInverse._31;
	InputData.mDir.y  = mouseRay.x*ViewInverse._12 + mouseRay.y*ViewInverse._22 + mouseRay.z*ViewInverse._32;
	InputData.mDir.z  = mouseRay.x*ViewInverse._13 + mouseRay.y*ViewInverse._23 + mouseRay.z*ViewInverse._33;
	InputData.mOrigin.x = ViewInverse._41;
	InputData.mOrigin.y = ViewInverse._42;
	InputData.mOrigin.z = ViewInverse._43;
}

