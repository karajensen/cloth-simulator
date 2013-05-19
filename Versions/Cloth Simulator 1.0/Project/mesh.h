/********************************************************************
 File: mesh.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: Makes use of GFXloader
 *******************************************************************/

#ifndef GFXMODEL_H
#define GFXMODEL_H


#include "includes.h"
#include "GFXLoader.h"
#include "transform.h"
#include "shader.h"
#include "collision.h"

class MESH : public TRANSFORM
{
protected:

	bool DrawMesh;
	int Index;
	LPD3DXMESH	m_pMesh;
	GFXLOADER m_Loader;
	LPDIRECT3DTEXTURE9	m_texture;
	SHADER * m_shader;
	D3DXHANDLE m_Technique;

	bool HasCollisions;
	COLLISION_SPHERE * s_collision;
	COLLISION_BOX * b_collision;

	float timer;
	float randomNumber;
	int MAX_RAND_RANGE;

public:

	MESH();
	virtual ~MESH();
	virtual bool OnLoad(WCHAR const* wszFilename, WCHAR const* wszDirectory, SHADER * meshFX, int index = 0, D3DXHANDLE technique = NULL);
	virtual void OnDraw();
	
	void MousePickingTest();
	int GetIndex(){return Index;};
	bool IsVisible(){return DrawMesh;};
	void SetVisibility(bool draw){DrawMesh = draw;};
	void SetTechnique(D3DXHANDLE tech){m_Technique = tech;};
	void SetTexture(int index = -100);
	void AddTexture(char * texDiffuse);
	void CreateNormalsForDetailMapping();

	//collisions
	void CreateBoundingBox(float Width, float Height, float Depth, D3DXVECTOR3 Location, bool HasMesh);
	void CreateBoundingSphere(float Radius, D3DXVECTOR3 Location, bool HasMesh);
	bool CollisionTest(MESH * mesh);
	bool CollisionTest(COLLISION_SPHERE * sphere);
	bool CollisionTest(COLLISION_BOX * box);
	COLLISION_SPHERE * GetCollisionSphere(){return s_collision;};
	COLLISION_BOX * GetCollisionBox(){return b_collision;};

	//transformations
	virtual void Transform(float sx, float sy, float sz, float rx, float ry, float rz, float tx, float ty, float tz);
	virtual void Rotate(float rx, float ry, float rz);
	virtual void Scale(float sx, float sy, float sz);
	virtual void Translate(float tx, float ty, float tz);
	virtual void MoveToPosition(float x, float y, float z);
	virtual void SetWorldMatrix(D3DXMATRIX & wmat);
	virtual void MoveToVectorPosition(D3DXVECTOR3 & pos);

};



#endif