
#include "global.h"
#include "mesh.h"
#include "GFXLoader.h"

//=============================================================
//Render the mesh
//=============================================================
void MESH::OnDraw()
{
	//CHECK OKAY TO PROCEED
	if(!m_pMesh)
		return;
	if(!DrawMesh)
		return;

	//SET SHADER
	LPD3DXEFFECT pEffect;
	if(SHADER::UseWorldShader())
	{
		//USING WORLD SHADER
		pEffect = SHADER::GetWorldShader();
		pEffect->SetTechnique("MAIN");
	}	
	else
	{
		//USING INDIVIDUAL SHADERS
		pEffect = m_shader->GetShader();

		//CHECK IF SPECIFIC TECHNIQUE
		if(m_Technique != NULL)
			pEffect->SetTechnique(m_Technique);
		else
			pEffect->SetTechnique("MAIN");
	}

	//TIMER
	if(timer != NULL)
	{
		timer += 0.08f;
		pEffect->SetFloat("Timer", timer);
	}

	//SET RANDOM NUMBER
	if(randomNumber != NULL)
	{
		static int counter = 0;
		counter ++;
		if(counter >= 2)
		{
			int r = rand() % MAX_RAND_RANGE; //for 0 to MAX_RANGE
		}
		pEffect->SetFloat("offset", randomNumber);
	}

	//SET TEXTURES
	if(m_texture != NULL)
		pEffect->SetTexture("texColor",m_texture);

	//SEND Camera
	pEffect->SetFloatArray("CameraPos", &(Camera->GetPos().x), 3);

	//SEND LIGHTS/SHADOWS
	Game.SendAllLightsToShader(pEffect); //To fix

	//SEND TRANSFORMATIONS
	D3DXMATRIX wvp = worldMatrix * gwd.matCurrentView * gwd.matCurrentProjection;
	D3DXMATRIX wit;
	float det = D3DXMatrixDeterminant(&worldMatrix);
	D3DXMatrixInverse(&wit, &det, &worldMatrix);
	D3DXMatrixTranspose(&wit, &wit);
	pEffect->SetMatrix("WorldInvTrans", &wit);
	pEffect->SetMatrix("WorldViewProjection", &wvp);
	pEffect->SetMatrix("World", &worldMatrix);

	//RENDER MESH
	UINT nPasses = 0;
	pEffect->Begin(&nPasses, 0);


	//Render the mesh
	for( UINT iPass = 0; iPass<nPasses; iPass++)
	{
		pEffect->BeginPass(iPass);
			m_pMesh->DrawSubset(0);
		pEffect->EndPass();
	}
	pEffect->End();

	//RENDER COLLLISION MESH
	if(s_collision != NULL)
		s_collision->OnDraw(worldMatrix);
	else if(b_collision != NULL)
		b_collision->OnDraw(worldMatrix);
}

//=============================================================
//Test collision with unknown collision shape
//=============================================================
bool MESH::CollisionTest(MESH * mesh)
{ 
	if(!DrawMesh)
		return false;

	if((s_collision != NULL) && (mesh->s_collision != NULL)) //check for sphere x sphere
		return COLLISION_MANAGER::TestCollision(s_collision,mesh->s_collision);
	else if((s_collision != NULL) && (mesh->b_collision != NULL)) //check for sphere x box
		return COLLISION_MANAGER::TestCollision(s_collision,mesh->b_collision);
	else if((b_collision != NULL) && (mesh->s_collision != NULL)) //check for sphere x box
		return COLLISION_MANAGER::TestCollision(mesh->s_collision,b_collision);
	else if((b_collision != NULL) && (mesh->b_collision != NULL))  //check for box x box
		return COLLISION_MANAGER::TestCollision(b_collision,mesh->b_collision);
	else
		return false;
}

//=============================================================
//Test collision with sphere
//=============================================================
bool MESH::CollisionTest(COLLISION_SPHERE * sphere)
{
	if(!DrawMesh)
		return false;

	if(s_collision != NULL)
		return COLLISION_MANAGER::TestCollision(sphere,s_collision);
	else if(b_collision != NULL)
		return COLLISION_MANAGER::TestCollision(sphere,b_collision);
	else
		return false;
}

//=============================================================
//Test collision with box
//=============================================================
bool MESH::CollisionTest(COLLISION_BOX * box)
{
	if(!DrawMesh)
		return false;

	if(s_collision != NULL)
		return COLLISION_MANAGER::TestCollision(s_collision,box);
	else if(b_collision != NULL)
		return COLLISION_MANAGER::TestCollision(b_collision,box);
	else
		return false;
}

//=============================================================
//Tranform mesh
//=============================================================
void MESH::Transform(float sx, float sy, float sz, float rx, float ry, float rz, float tx, float ty, float tz)
{
	TRANSFORM::Transform(sx,sy,sz,rx,ry,rz,tx,ty,tz);
	if(s_collision != NULL)
		s_collision->UpdateCollision(worldMatrix);
	else if(b_collision != NULL)
		b_collision->UpdateCollision(worldMatrix);
}

//=============================================================
//Rotate mesh
//=============================================================
void MESH::Rotate(float rx, float ry, float rz)
{
	TRANSFORM::Rotate(rx,ry,rz);
	D3DXVECTOR3 newpos = this->GetPosition();
	if(s_collision != NULL)
		s_collision->UpdateCollision(newpos);
	else if(b_collision != NULL)
		b_collision->UpdateCollision(newpos);
}

//=============================================================
//Scale
//=============================================================
void MESH::Scale(float sx, float sy, float sz)
{
	TRANSFORM::Scale(sx,sy,sz);
	if(s_collision != NULL)
		s_collision->UpdateCollision(worldMatrix);
	else if(b_collision != NULL)
		b_collision->UpdateCollision(worldMatrix);
}

//=============================================================
//Translate
//=============================================================
void MESH::Translate(float tx, float ty, float tz)
{
	TRANSFORM::Translate(tx,ty,tz);
	D3DXVECTOR3 newpos = this->GetPosition();
	if(s_collision != NULL)
		s_collision->UpdateCollision(newpos);
	else if(b_collision != NULL)
		b_collision->UpdateCollision(newpos);
}

//=============================================================
//Move to a position
//=============================================================
void MESH::MoveToPosition(float x, float y, float z)
{
	TRANSFORM::MoveToPosition(x,y,z);
	D3DXVECTOR3 newpos = this->GetPosition();
	if(s_collision != NULL)
		s_collision->UpdateCollision(newpos);
	else if(b_collision != NULL)
		b_collision->UpdateCollision(newpos);
}

//=============================================================
//Move to a position
//=============================================================
void MESH::MoveToVectorPosition(D3DXVECTOR3 & pos)
{
	TRANSFORM::MoveToVectorPosition(pos);
	D3DXVECTOR3 newpos = this->GetPosition();
	if(s_collision != NULL)
		s_collision->UpdateCollision(newpos);
	else if(b_collision != NULL)
		b_collision->UpdateCollision(newpos);
}

//=============================================================
//Set the world matrix
//=============================================================
void MESH::SetWorldMatrix(D3DXMATRIX & wmat)
{
	TRANSFORM::SetWorldMatrix(wmat);
	D3DXVECTOR3 newpos = this->GetPosition();
	if(s_collision != NULL)
		s_collision->UpdateCollision(newpos);
	else if(b_collision != NULL)
		b_collision->UpdateCollision(newpos);
}

//=============================================================
//Check mesh against mouse click
//=============================================================
void MESH::MousePickingTest()
{
	//only check if mesh is visible
	if(DrawMesh)
	{

		LPD3DXMESH meshToTest;
		D3DXMATRIX world = worldMatrix;

		if(s_collision != NULL) //USE SPHERE COLLISION MESH
		{
			meshToTest = s_collision->GetMesh();
		}
		else if(b_collision != NULL) //USE BOX COLLISION MESH
		{
			meshToTest = b_collision->GetMesh();
		}
		else //USE ACTUAL MESH
		{
			int index = Index; //for testing whether which meshes don't yet have bounding box
			meshToTest = m_pMesh;
		}

		D3DXMATRIX worldInverse;
		D3DXMatrixInverse(&worldInverse,NULL,&world);
		D3DXVECTOR3 rayObjOrigin = InputData.mOrigin;
		D3DXVECTOR3 rayObjDirection = InputData.mDir;
			
		D3DXVec3TransformCoord(&rayObjOrigin,&rayObjOrigin,&worldInverse);
		D3DXVec3TransformNormal(&rayObjDirection,&rayObjDirection,&worldInverse);
		D3DXVec3Normalize(&rayObjDirection,&rayObjDirection);

		BOOL hasHit; float distanceToCollision;
		if(FAILED(D3DXIntersect(meshToTest, &rayObjOrigin, &rayObjDirection, &hasHit, NULL, NULL, NULL, &distanceToCollision, NULL, NULL)))
			hasHit = false;

		if(hasHit)
		{
			//if first test or if better than a previous test distance wise...
			if((InputData.meshClicked == NULL) || (distanceToCollision < InputData.distanceToMeshClicked))
			{
				InputData.meshClicked = this;
				InputData.distanceToMeshClicked = distanceToCollision;
			}
		}
	}
}

//=============================================================
//load texture set
//=============================================================
void MESH::AddTexture(char * texDiffuse)
{	
	if(FAILED(D3DXCreateTextureFromFile(d3ddev, TEXT(texDiffuse), &m_texture)))
		exit(EXIT_FAILURE);
}
//=============================================================
//Set current texture
//=============================================================
void MESH::SetTexture(int index)
{

}