/********************************************************************
 File: transform.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: Class for moving objects in world space
 *******************************************************************/

#ifndef TRANSFORM_H_
#define TRANSFORM_H_


#include "includes.h"

class TRANSFORM
{
protected:
	D3DXMATRIX worldMatrix;
public:
	TRANSFORM(){ D3DXMatrixIdentity(&worldMatrix); };
	virtual void Transform(float sx, float sy, float sz, float rx, float ry, float rz, float tx, float ty, float tz);
	virtual void Rotate(float rx, float ry, float rz);
	virtual void Scale(float sx, float sy, float sz);
	virtual void Translate(float tx, float ty, float tz);
	virtual void MoveToPosition(float x, float y, float z);
	virtual void MoveToVectorPosition(D3DXVECTOR3 & pos);
	virtual void SetWorldMatrix(D3DXMATRIX & wmat){worldMatrix = wmat;};
	virtual ~TRANSFORM(){}

	//Reference to DirectX Sample Browser: Shadows
	D3DXMATRIX   GetWorldMatrix()	{ return worldMatrix; };
	D3DXVECTOR3  GetWorldRight()	{ return D3DXVECTOR3(worldMatrix._11,worldMatrix._12,worldMatrix._13); };
	D3DXVECTOR3  GetWorldUp()		{ return D3DXVECTOR3(worldMatrix._21,worldMatrix._22,worldMatrix._23); };
    D3DXVECTOR3  GetWorldAhead()	{ return D3DXVECTOR3(worldMatrix._31,worldMatrix._32,worldMatrix._33); };
    D3DXVECTOR3  GetPosition()		{ return D3DXVECTOR3(worldMatrix._41,worldMatrix._42,worldMatrix._43); };
};

#endif