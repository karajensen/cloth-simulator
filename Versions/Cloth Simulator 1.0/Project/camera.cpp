
#include "global.h"

//=============================================================
//set up initial position of Camera
//=============================================================
CAMERA::CAMERA(float x, float y, float z)
{
	initialPos = D3DXVECTOR3(x,y,z);
	this->ResetCamera();
}

//=============================================================
//Reset Camera
//=============================================================
void CAMERA::ResetCamera()
{
	reset = true;
	MovedForward = false;
	MovedBackward = false;

	//set the Camera position
	pos = initialPos; 

	//set axis to initial values
	up = D3DXVECTOR3(0.0f,1.0f,0.0f);
	forward = D3DXVECTOR3(0.0f,0.0f,1.0f);
	right = D3DXVECTOR3(1.0f,0.0f,0.0f);

	//initialise rotation
	yaw = 0;
	roll = 0;
	pitch = 0;

	//initialise speed
	movSpeed = 0.4f;
	rotSpeed = 0.015f;
}

//=============================================================
//Initialise the projection matrix
//=============================================================
void CAMERA::CreateProjMatrix()
{
    D3DXMatrixPerspectiveFovLH(&matProjection,
                               D3DX_PI/4, //horizontal field of view
                               (FLOAT)gwd.WINDOW_WIDTH / (FLOAT)gwd.WINDOW_HEIGHT, //aspect ratio
                               1.0f, //the near view-plane
                               1000.0f); //the far view-plane
}

//=============================================================
//set the view matrix
//=============================================================
void CAMERA::CreateViewMatrix()
{
	if (ViewNeedsUpdating)
	{
		//set axis back to initial values(set due to float-pt errors over time would screw the axis's orthogonality)
		up = D3DXVECTOR3(0.0f,1.0f,0.0f);
		forward = D3DXVECTOR3(0.0f,0.0f,1.0f);
		right = D3DXVECTOR3(1.0f,0.0f,0.0f);

		//Create yaw rotation
		D3DXMATRIX yawMatrix;
		D3DXMatrixRotationAxis(&yawMatrix, &up, yaw); //matrix to store in, axis, angle
		//rotate axis using yaw rotation around up axis
		D3DXVec3TransformCoord(&forward, &forward, &yawMatrix); 
		D3DXVec3TransformCoord(&right, &right, &yawMatrix); 

		//Create pitch rotation
		D3DXMATRIX pitchMatrix;
		D3DXMatrixRotationAxis(&pitchMatrix, &right, pitch);
		//rotate axis using pitch rotation around right axis
		D3DXVec3TransformCoord(&forward, &forward, &pitchMatrix); 
		D3DXVec3TransformCoord(&up, &up, &pitchMatrix); 

		//Create roll rotation
		D3DXMATRIX rollMatrix;
		D3DXMatrixRotationAxis(&rollMatrix, &forward, roll);
		//rotation axis using roll rotation around forward axis
		D3DXVec3TransformCoord(&right, &right, &rollMatrix); 
		D3DXVec3TransformCoord(&up, &up, &rollMatrix); 
		
		//set to identity matrix
		D3DXMatrixIdentity(&matView);

		//fill in view matrix
		matView._11 = right.x;	matView._12 = up.x;	  matView._13 = forward.x;
		matView._21 = right.y;	matView._22 = up.y;   matView._23 = forward.y;
		matView._31 = right.z;	matView._32 = up.z;   matView._33 = forward.z;
		
		//fill in view matrix position vector
		matView._41 = - D3DXVec3Dot(&pos,&right); 
		matView._42 = - D3DXVec3Dot(&pos,&up);
		matView._43 = - D3DXVec3Dot(&pos,&forward);

		//fill in world matrix
		D3DXMATRIX matTrans, matRX, matRY, matRZ;
		D3DXMatrixRotationX(&matRX, pitch); //PITCH
		D3DXMatrixRotationY(&matRY, yaw); //YAW
		D3DXMatrixRotationZ(&matRZ, roll); //ROLL
		matWorld = (matRZ*matRX*matRY);
		matWorld._41 = pos.x;
		matWorld._42 = pos.y;
		matWorld._43 = pos.z;

	}
}

//=============================================================
//move the Camera forward
//=============================================================
void CAMERA::Forward(float val)
{
	if(val < 0)
		MovedBackward = true;
	else
		MovedForward = true;

	pos += forward*val;
	ViewNeedsUpdating = true;
}

//=============================================================
//move the Camera to the right
//=============================================================
void CAMERA::Right(float val)
{
	pos += right*val;
	ViewNeedsUpdating = true;
}

//=============================================================
//move the Camera up
//=============================================================
void CAMERA::Up(float val)
{
	pos += up*val;
	ViewNeedsUpdating = true;
}

//=============================================================
//move the Camera around Y axis
//=============================================================
void CAMERA::Yaw(float angle)
{
	//yaw += DEGTORAD(angle);
	yaw += angle;
	ViewNeedsUpdating = true;
}

//=============================================================
//move the Camera around X axis
//=============================================================
void CAMERA::Pitch(float angle)
{
	//pitch += DEGTORAD(angle);
	pitch += angle;
	ViewNeedsUpdating = true;
}

//=============================================================
//move the Camera around Z axis
//=============================================================
void CAMERA::Roll(float angle)
{
	//roll += DEGTORAD(angle);
	roll += angle;
	ViewNeedsUpdating = true;
}