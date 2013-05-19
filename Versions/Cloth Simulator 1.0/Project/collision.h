/********************************************************************
 File: collision.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments:	Code references from Toymaker
			Additional Code references in .cpp
			No support for squewed scaling for bounding sphere
			No support for rotating bounding box
 *******************************************************************/


#ifndef COLLISION_H_
#define COLLISION_H_


#include "includes.h"
#include "shader.h"


//MAIN COLLISION BASE CLASS
class COLLISION
{
protected:
	LPD3DXMESH mesh;
	static SHADER boundShader;
	D3DXVECTOR3 boundColor;

public:
	D3DXVECTOR3 Center;

	COLLISION(){ mesh = NULL; boundColor = D3DXVECTOR3(0,0,1.0); };
	LPD3DXMESH GetMesh(){return mesh;};
	D3DXVECTOR3 & GetCenter(){return Center;};

	void OnDraw(D3DXMATRIX & worldMatrix, bool OverrideDraw = false);
	void OnDraw(bool OverrideDraw = false);
	void ReleaseCollision(){ if(mesh != NULL){ mesh->Release(); } }
	void SetBoundColor(D3DXVECTOR3 & color){boundColor = color;};
	D3DXVECTOR3 & GetBoundColor(){return boundColor;}

	virtual ~COLLISION(){}
	virtual void LoadMesh() = 0;
	virtual void UpdateCollision(D3DXMATRIX & worldMatrix) = 0;
	virtual void UpdateCollision(D3DXVECTOR3 & position) = 0;

	static void InitialiseCollisions();
	static bool DrawCollisions;
};


//BOX MESH
class COLLISION_BOX : public COLLISION
{
public:
	D3DXVECTOR3 minBounds,maxBounds;
	float width,depth,height,Scale;

	void Initialise(float Width, float Height, float Depth, D3DXVECTOR3 & Position, bool HasMesh);
	virtual void UpdateCollision(D3DXMATRIX & worldMatrix);
	virtual void UpdateCollision(D3DXVECTOR3 & position);
	virtual void LoadMesh();
	virtual ~COLLISION_BOX(){}
};

//SPHERE MESH
class COLLISION_SPHERE : public COLLISION
{
public:
	float Radius;
	float DrawRadius;
	float Scale;

	void Initialise(float radius, D3DXVECTOR3 & Position, bool HasMesh, float drawradius = 0);	
	virtual void UpdateCollision(D3DXMATRIX & worldMatrix);
	virtual void UpdateCollision(D3DXVECTOR3 & position);
	virtual void LoadMesh();
	virtual ~COLLISION_SPHERE(){};
};

class COLLISION_MANAGER
{
public:
	static bool TestCollision(COLLISION_BOX * BoxA, COLLISION_BOX * BoxB);
	static bool TestCollision(COLLISION_SPHERE * SphereA, COLLISION_SPHERE * SphereB);
	static bool TestCollision(COLLISION_SPHERE * Sphere, COLLISION_BOX * Box);
};

#endif