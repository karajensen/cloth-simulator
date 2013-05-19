
#include "collision.h"
#include "global.h"

SHADER COLLISION::boundShader;
bool COLLISION::DrawCollisions = false;

//=============================================================
//Initialise collisions
//=============================================================
void COLLISION::InitialiseCollisions()
{
	boundShader.OnLoad(".\\Resources\\bounds.fx");
}

//=============================================================
//Draw the mesh [creates own worldmatrix from center position]
//=============================================================
void COLLISION::OnDraw(bool OverrideDraw)
{
	if(DrawCollisions && (mesh!=NULL))
	{
		D3DXMATRIX world;
		D3DXMatrixIdentity(&world);
		world._41 = Center.x;
		world._42 = Center.y;
		world._43 = Center.z;
		OnDraw(world,OverrideDraw);
	}
}
//=============================================================
//Draw the mesh [specify the world matrix]
//=============================================================
void COLLISION::OnDraw(D3DXMATRIX & worldMatrix, bool OverrideDraw)
{
	if((DrawCollisions || OverrideDraw) && (mesh!=NULL))
	{
		//SET SHADER
		LPD3DXEFFECT pEffect;
		pEffect = boundShader.GetShader();
		pEffect->SetTechnique("MAIN");

		D3DXMATRIX world = worldMatrix;
		D3DXMATRIX wvp = world * gwd.matCurrentView * gwd.matCurrentProjection;
		pEffect->SetMatrix("WorldViewProjection", &wvp);

		pEffect->SetFloatArray("boundColor", &boundColor.x, 3);

		//DRAW MESH
		UINT nPasses = 0;
		pEffect->Begin(&nPasses, 0);
		for( UINT iPass = 0; iPass<nPasses; iPass++)
		{
			pEffect->BeginPass(iPass);
				mesh->DrawSubset(0);
			pEffect->EndPass();
		}
		pEffect->End();
	}
}

//=============================================================
//Specify custom collision box size
//=============================================================
void COLLISION_BOX::Initialise(float Width, float Height, float Depth, D3DXVECTOR3 & Position, bool HasMesh)
{
	width = Width;
	height = Height;
	depth = Depth;
	Center = Position;

	minBounds.x = Center.x - (width/2);
	minBounds.y = Center.y - (height/2);
	minBounds.z = Center.z - (depth/2);
	maxBounds.x = Center.x + (width/2);
	maxBounds.y = Center.y + (height/2);
	maxBounds.z = Center.z + (depth/2);

	if(HasMesh){this->LoadMesh();}
	else{mesh = NULL;}
	Scale = 1.0f;
}

//=============================================================
//Specify custom collision sphere size
//=============================================================
void COLLISION_SPHERE::Initialise(float radius, D3DXVECTOR3 & Position, bool HasMesh, float drawradius)
{
	if(drawradius <= 0)
		DrawRadius = radius;
	else
		DrawRadius = drawradius;
	Radius = radius;
	Center = Position;
	if(HasMesh){this->LoadMesh();}
	else{mesh = NULL;}
	Scale = 1.0f;
}

//=============================================================
//Load the sphere mesh
//=============================================================
void COLLISION_SPHERE::LoadMesh()
{
	if(mesh != NULL)
		mesh->Release();
	D3DXCreateSphere(d3ddev,DrawRadius,8,8,&mesh,NULL);
}

//=============================================================
//Load the box mesh
//=============================================================
void COLLISION_BOX::LoadMesh()
{
	if(mesh != NULL)
		mesh->Release();
	D3DXCreateBox(d3ddev,width,height,depth,&mesh,NULL);
}

//=============================================================
//Update the box collision from world Matrix [supports scaling]
//=============================================================
void COLLISION_BOX::UpdateCollision(D3DXMATRIX & worldMatrix)
{
	Center = D3DXVECTOR3(worldMatrix._41,worldMatrix._42,worldMatrix._43);
	float NewScale = (worldMatrix._11*worldMatrix._11) + (worldMatrix._21*worldMatrix._21) + (worldMatrix._31*worldMatrix._31);
	if(NewScale != (pow(Scale,2)))
	{
		Scale = NewScale;
		width = width * Scale;
		height = height * Scale;
		depth = depth * Scale;
	}
	minBounds.x = Center.x - (width/2);
	minBounds.y = Center.y - (height/2);
	minBounds.z = Center.z - (depth/2);
	maxBounds.x = Center.x + (width/2);
	maxBounds.y = Center.y + (height/2);
	maxBounds.z = Center.z + (depth/2);
}

//=============================================================
//Update the box collision from position only [no scaling]
//=============================================================
void COLLISION_BOX::UpdateCollision(D3DXVECTOR3 & position)
{
	Center = position;
	minBounds.x = Center.x - (width/2);
	minBounds.y = Center.y - (height/2);
	minBounds.z = Center.z - (depth/2);
	maxBounds.x = Center.x + (width/2);
	maxBounds.y = Center.y + (height/2);
	maxBounds.z = Center.z + (depth/2);
}

//=============================================================
//Update the sphere collision from world matrix [supports scaling]
//=============================================================
void COLLISION_SPHERE::UpdateCollision(D3DXMATRIX & worldMatrix)
{
	Center = D3DXVECTOR3(worldMatrix._41,worldMatrix._42,worldMatrix._43);
	float NewScale = (worldMatrix._11*worldMatrix._11) + (worldMatrix._21*worldMatrix._21) + (worldMatrix._31*worldMatrix._31);
	if(NewScale != (pow(Scale,2)))
	{
		Radius = Radius * NewScale;
		Scale = NewScale;
	}
}

//=============================================================
//Update the sphere collision from position only [no scaling]
//=============================================================
void COLLISION_SPHERE::UpdateCollision(D3DXVECTOR3 & position)
{
	Center = position;
}

//=============================================================
//Test whether two boxes collide
//=============================================================
bool COLLISION_MANAGER::TestCollision(COLLISION_BOX * BoxA, COLLISION_BOX * BoxB)
{
	if( BoxA->maxBounds.x < BoxB->minBounds.x || BoxA->minBounds.x > BoxB->maxBounds.x )
		return false;
	if( BoxA->maxBounds.y < BoxB->minBounds.y || BoxA->minBounds.y > BoxB->maxBounds.y )
		return false;
	if( BoxA->maxBounds.z < BoxB->minBounds.z || BoxA->minBounds.z > BoxB->maxBounds.z )
		return false;
	else
		return true;
}

//=============================================================
//Test whether two sphere collide
//=============================================================
bool COLLISION_MANAGER::TestCollision(COLLISION_SPHERE * SphereA, COLLISION_SPHERE * SphereB)
{
	D3DVECTOR relPos = SphereA->Center - SphereB->Center;
	float dist = (relPos.x * relPos.x) + (relPos.y * relPos.y) + (relPos.z * relPos.z);
	float minDist = SphereA->Radius + SphereB->Radius;
	if(dist <= (pow(minDist,2)))
		return true;
	else
		return false;
}

//=============================================================
//Test whether a sphere and box collide
//=============================================================
bool COLLISION_MANAGER::TestCollision(COLLISION_SPHERE * Sphere, COLLISION_BOX * Box)
{
	//get normalized vector from sphere center to box center
	//times by radius to get vector from center sphere to sphere pointing at box
	D3DXVECTOR3 SphereToBox = Box->Center - Sphere->Center;
	D3DXVec3Normalize(&SphereToBox,&SphereToBox);
	D3DXVECTOR3 SphereVector = Sphere->Radius * SphereToBox; //vector from center of sphere pointing at box

	//convert vector to world space
	SphereVector = SphereVector + Sphere->Center;

	//check for collision
	if((SphereVector.x < Box->maxBounds.x) && (SphereVector.x > Box->minBounds.x)) //test if in x portion
		if((SphereVector.y < Box->maxBounds.y) && (SphereVector.y > Box->minBounds.y)) //test if in y portion
			if((SphereVector.z < Box->maxBounds.z) && (SphereVector.z > Box->minBounds.z)) //test if in z portion
				return true;
	return false;
}