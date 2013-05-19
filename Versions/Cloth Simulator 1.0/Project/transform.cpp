
#include "global.h"
#include "transform.h"

//=============================================================
//transform the mesh
//=============================================================
void TRANSFORM::Transform(float sx, float sy, float sz, float rx, float ry, float rz, float tx, float ty, float tz)
{
	D3DXMATRIX matTrans, matRX, matRY, matRZ, matScal, matRot;
	
	//scale
	D3DXMatrixScaling(&matScal, sx, sy, sz);

	//rotation
	D3DXMatrixRotationX(&matRX, DEGTORAD(rx)); //PITCH
	D3DXMatrixRotationY(&matRY, DEGTORAD(ry)); //YAW
	D3DXMatrixRotationZ(&matRZ, DEGTORAD(rz)); //ROLL
	matRot = (matRZ*matRX*matRY);

	//translation
	D3DXMatrixTranslation(&matTrans, tx, ty, tz); 

	//ORDER: SCALE/ROTATE/TRANSLATE
	//worldMatrix = (matScal*matRot*matTrans)*worldMatrix; //if position is stored in forth col
	worldMatrix = (matTrans*matRot*matScal)*worldMatrix; //if position is stored in forth row

}

//=============================================================
//only rotate the mesh
//=============================================================
void TRANSFORM::Rotate(float rx, float ry, float rz)
{
	D3DXMATRIX matRX, matRY, matRZ;
	D3DXMatrixRotationX(&matRX, DEGTORAD(rx)); //PITCH
	D3DXMatrixRotationY(&matRY, DEGTORAD(ry)); //YAW
	D3DXMatrixRotationZ(&matRZ, DEGTORAD(rz)); //ROLL
	worldMatrix = (matRZ*matRX*matRY)*worldMatrix;
}

//=============================================================
//only scale the mesh
//=============================================================
void TRANSFORM::Scale(float sx, float sy, float sz)
{
	D3DXMATRIX matScal;
	D3DXMatrixScaling(&matScal, sx, sy, sz);
	worldMatrix = matScal*worldMatrix;
}

//=============================================================
//only translate the mesh
//=============================================================
void TRANSFORM::Translate(float tx, float ty, float tz)
{
	D3DXMATRIX matTrans;
	D3DXMatrixTranslation(&matTrans, tx, ty, tz); 
	worldMatrix = matTrans*worldMatrix;
}

//=============================================================
//move mesh to specific position
//=============================================================
void TRANSFORM::MoveToPosition(float x, float y, float z)
{
	worldMatrix._41 = x;
	worldMatrix._42 = y;
	worldMatrix._43 = z;
}

//=============================================================
//move mesh to specific position
//=============================================================
void TRANSFORM::MoveToVectorPosition(D3DXVECTOR3 & pos)
{
	worldMatrix._41 = pos.x;
	worldMatrix._42 = pos.y;
	worldMatrix._43 = pos.z;
}