
#include "global.h"
#include "cloth.h"
//=============================================================
//Toggle whether simulation happens
//=============================================================
void CLOTH::ToggleSimulation()
{
	if(Simulate)
		Simulate = false;
	else
		Simulate = true;
}
//=============================================================
//Toggle whether simulation happens
//=============================================================
void CLOTH::ToggleShowVerts()
{
	if(DrawVerts)
		DrawVerts = false;
	else
		DrawVerts = true;
}
//=============================================================
//Add a force to all particles
//=============================================================
void CLOTH::UnpinCloth()
{
	for(int i = 0; i < VertCount; i++)
	{
		particles[i].PinParticle(false);
		vertices[i].SetBoundColor(D3DXVECTOR3(0,0,1.0f));
	}
	if(Manipulate)
		SelectRow(SelectedRow);
}
//=============================================================
//Add a force to all particles
//=============================================================
void CLOTH::AddForce(FLOAT3 & force)
{
	for(int i = 0; i < VertCount; i++)
		particles[i].AddForce(force); 
}
//=============================================================
//For each timestep
//=============================================================
void CLOTH::OnTimeStep()
{
	if(!Simulate && !Manipulate)
		return;
	if(Simulate)
		AddForce(FLOAT3(0,-0.2f,0)*TIMESTEP_SQUARED);
	
	//fix springs
	for(int j = 0; j < SpringIterations; j++)
	{
		for(int i = 0; i < SpringCount; i++)
			springs[i].SolveSpring();
	}
	//update vertex containers
	for(int i = 0; i < VertCount; i++)
	{
		particles[i].OnTimeStep();
		vertexData[i].vPosition = D3DXVECTOR3(particles[i].position.x,particles[i].position.y,particles[i].position.z);
		vertices[i].UpdateCollision(vertexData[i].vPosition);
	}
	//update the vertex buffer
	UpdateNormals();
	UpdateVertexBuffer();
}
//=============================================================
//Reset the cloth
//=============================================================
void CLOTH::Reset()
{
	Simulate = false;
	for(int i = 0; i < VertCount; i++)
	{
		particles[i].position = particles[i].old_position = particles[i].initial_position;
		vertexData[i].vPosition = D3DXVECTOR3(particles[i].initial_position.x,particles[i].initial_position.y,particles[i].initial_position.z);
		vertices[i].UpdateCollision(vertexData[i].vPosition);
	}
	UpdateNormals();
	UpdateVertexBuffer();
}
//=============================================================
//Update the vertex buffer
//=============================================================
void CLOTH::UpdateVertexBuffer()
{
	//Lock the vertex buffer
	if(FAILED(m_pMesh->LockVertexBuffer(0,&pvVoid)))
	{
		MessageBox(NULL, TEXT("Vertex buffer lock failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}

	//Copy over values 
	#pragma warning(disable: 4996)
	std::copy(vertexData.begin(), vertexData.end(),LPVERTEX(pvVoid));

	//unlock vertex buffer
	m_pMesh->UnlockVertexBuffer();
}
//=============================================================
//Update normals
//=============================================================
void CLOTH::UpdateNormals()
{
	//reset normals
	for(int i = 0; i < VertCount; i++)
		vertexData[i].vNormal = D3DXVECTOR3(0,0,0);

	//create smooth per particle normals
	for(int x = 0; x < (VertWidth-1); x++)
	{
		for(int y=0; y < (VertLength-1); y++)
		{
			D3DXVECTOR3 normal = CalculateTriNormal(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1));
			getVertex(x+1,y)->vNormal = getVertex(x+1,y)->vNormal + normal;
			getVertex(x,y)->vNormal = getVertex(x,y)->vNormal + normal;
			getVertex(x,y+1)->vNormal = getVertex(x,y+1)->vNormal + normal;

			normal = CalculateTriNormal(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1));
			getVertex(x+1,y+1)->vNormal = getVertex(x+1,y+1)->vNormal + normal;
			getVertex(x+1,y)->vNormal = getVertex(x+1,y)->vNormal + normal;
			getVertex(x,y+1)->vNormal = getVertex(x,y+1)->vNormal + normal;
		}
	}
}

//=============================================================
//Load the mesh (CLOTH)
//=============================================================
bool CLOTH::OnLoad(int width, int length, float scale, SHADER * meshFX)
{

	//CREATE VERTEX DEFINITION
	D3DVERTEXELEMENT9 VERTEX_DECL[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },		
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	//SET VALUES
	SelectedRow = 1;
	D3DXMatrixIdentity(&worldMatrix);
	DrawMesh = true;
	DrawVerts = true;
	Simulate = false;
	SpringIterations = 4;
	m_shader = meshFX;
	int faceno = ((width-1)*(length-1))*2;
	VertCount = length*width;	
	VertWidth = width;
	VertLength = length;
	Manipulate = false;
	SelfCollide = false;

	//create verticies
	int minW = -width/2;
	int maxW = minW + width;
	int minL = -length/2;
	int maxL = minL + length;
	float startingheight = 8.0f;
	float UVu = 0;
	float UVv = 0;
	for(int x = minW; x < maxW; x++)
	{
		for(int y = minL; y < maxL; y++)
		{
			VERTEX temp;
			temp.vPosition = D3DXVECTOR3((float(x)*scale),startingheight,(float(y)*scale));
			temp.vTexCoord = D3DXVECTOR2(UVu,UVv);
			vertexData.push_back(temp);
			UVu += 0.5;
		}
		UVu = 0;
		UVv +=0.5;
	}

	//create indices [alogrithm from Frank Luna]
	for(int i = 0; i < (faceno*3); i++)
		indexData.push_back(DWORD(0));

    int k = 0;
    for(int i = 0; i < width-1; ++i)
    {
        for(int j = 0; j < length-1; ++j)
        {
            indexData[k]   = i*length+j;
            indexData[k+1] = i*length+j+1;
            indexData[k+2] = (i+1)*length+j;
            indexData[k+3] = (i+1)*length+j;
            indexData[k+4] = i*length+j+1;
            indexData[k+5] = (i+1)*length+j+1;
            k += 6; // next quad
        }
	}

	//create particles
	for(int i = 0; i < VertCount; i++)
	{
		particles.push_back(PARTICLE());
		particles[i].position.x = vertexData[i].vPosition.x;
		particles[i].position.y = vertexData[i].vPosition.y;
		particles[i].position.z = vertexData[i].vPosition.z;
		particles[i].InitialiseParticle();
	}
	//Connect neighbouring particles with springs
	for(int x = 0; x < width; x++)
	{
		for(int y = 0; y < length; y++)
		{
			if (x<width-1)
			{ springs.push_back(SPRING(getParticle(x,y),getParticle(x+1,y)));}
			if (y<length-1) 
			{ springs.push_back(SPRING(getParticle(x,y),getParticle(x,y+1)));}
			if (x<width-1 && y<length-1) 
			{ springs.push_back(SPRING(getParticle(x,y),getParticle(x+1,y+1)));}
			if (x<width-1 && y<length-1) 
			{ springs.push_back(SPRING(getParticle(x+1,y),getParticle(x,y+1)));}
		}
	}
	//Connect bending springs
	for(int x = 0; x < width; x++)
	{
		for(int y = 0; y < length; y++)
		{
			if (x<width-2) 
			{ springs.push_back(SPRING(getParticle(x,y),getParticle(x+2,y)));}
			if (y<length-2)
			{ springs.push_back(SPRING(getParticle(x,y),getParticle(x,y+2)));}
			if (x<width-2 && y<length-2) 
			{ springs.push_back(SPRING(getParticle(x,y),getParticle(x+2,y+2)));}
			if (x<width-2 && y<length-2) 
			{ springs.push_back(SPRING(getParticle(x+2,y),getParticle(x,y+2)));}
		}
	}
	SpringCount = int(springs.size());

	//create collision spheres
	float colSize = scale/2; 
	for(int i = 0; i < VertCount; i++)
		vertices.push_back(COLLISION_SPHERE());
	for(int i = 0; i < VertCount; i++)
		vertices[i].Initialise(colSize,vertexData[i].vPosition,true,0.1f);

	//create normals
	UpdateNormals();

	//Create the mesh
	if(m_pMesh == NULL)
	{
		if( FAILED(	D3DXCreateMesh(faceno, vertexData.size(),
								   D3DXMESH_VB_DYNAMIC | D3DXMESH_IB_MANAGED | D3DXMESH_32BIT,
								   VERTEX_DECL,d3ddev,&m_pMesh) ) )
		{
			MessageBox(NULL, TEXT("Mesh creation failed"), TEXT("ERROR"), MB_OK);
			exit(EXIT_FAILURE);
		}
	}

	//Vertex Buffer
	if(FAILED(m_pMesh->LockVertexBuffer(0,&pvVoid)))
	{
		MessageBox(NULL, TEXT("Vertex buffer lock failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}
	#pragma warning(disable: 4996)
	std::copy(vertexData.begin(), vertexData.end(),LPVERTEX(pvVoid));
	m_pMesh->UnlockVertexBuffer();

	//Index Buffer
	if(FAILED(m_pMesh->LockIndexBuffer(0,&piVoid)))
	{
		MessageBox(NULL, TEXT("Index buffer lock failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}
	#pragma warning(disable: 4996)
	std::copy(indexData.begin(), indexData.end(), (DWORD*)piVoid);
	m_pMesh->UnlockIndexBuffer();

	
	//FINISH
	return TRUE;
}
//=============================================================
//Release the cloth
//=============================================================
void CLOTH::ReleaseCloth()
{
	for(unsigned int i = 0; i < vertices.size(); i++)
		vertices[i].ReleaseCollision();
	
	if(m_texture != NULL) { m_texture->Release(); }
	if(m_pMesh != NULL) { m_pMesh->Release(); }
}
//=============================================================
//Render the mesh
//=============================================================
void CLOTH::OnDraw()
{
	//CHECK OKAY TO PROCEED
	if(!DrawMesh)
		return;

	//SET SHADER
	LPD3DXEFFECT pEffect;
	pEffect = m_shader->GetShader();
	pEffect->SetTechnique("MAIN");

	//SET TEXTURES
	if(m_texture != NULL)
		pEffect->SetTexture("texColor",m_texture);
	
	//SEND Camera
	pEffect->SetFloatArray("CameraPos", &(Camera->GetPos().x), 3);

	//SEND LIGHTS/SHADOWS
	Game.SendAllLightsToShader(pEffect);

	//SEND TRANSFORMATIONS
	D3DXMATRIX wvp = gwd.matCurrentView * gwd.matCurrentProjection;
	pEffect->SetMatrix("WorldInvTrans", &worldMatrix);
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

	//render collisions
	if(DrawVerts)
	{
		for(int i = 0; i < VertCount; i++)
			vertices[i].OnDraw(true);
	}
}
//=============================================================
//Calculate cloth normals
//=============================================================
D3DXVECTOR3 CLOTH::CalculateTriNormal(PARTICLE * p1, PARTICLE * p2, PARTICLE * p3)
{
	FLOAT3 pos1 = p1->position;
	FLOAT3 pos2 = p2->position;
	FLOAT3 pos3 = p3->position;
	FLOAT3 v1 = pos2-pos1;
	FLOAT3 v2 = pos3-pos1;
	FLOAT3 cross = v1.Cross(v2);
	cross.Normalize();
	return D3DXVECTOR3(cross.x,cross.y,cross.z);
}
//=============================================================
//Test for collisions with objects
//=============================================================
void CLOTH::TestSphereCollision(COLLISION_SPHERE * sphere)
{
	if(sphere != NULL)
	{
		FLOAT3 sphereCenter;
		sphereCenter.x = sphere->GetCenter().x;
		sphereCenter.y = sphere->GetCenter().y;
		sphereCenter.z = sphere->GetCenter().z;

		for(int i = 0; i < VertCount; i++)
		{
			FLOAT3 centerToParticle = particles[i].position - sphereCenter;
			float centerToParticle_l = centerToParticle.Length();
			if (centerToParticle_l < sphere->Radius)
				particles[i].MovePosition(centerToParticle.Normalize()*(sphere->Radius-centerToParticle_l)); 
		}
	}
}
//=============================================================
//Test for collisions with objects
//=============================================================
void CLOTH::TestBoxCollision(COLLISION_BOX * box)
{
	if(box != NULL)
	{
		for(int i = 0; i < VertCount; i++)
		{
			FLOAT3 pos = particles[i].position;
			if(pos.y <= box->maxBounds.y)
			{
				float distance = box->maxBounds.y-pos.y;
				if(distance < 0)
					distance*=-1;
				particles[i].MovePosition(FLOAT3(0,distance,0));
			}
		}
	}
}
//=============================================================
//Self Collision tests
//=============================================================
bool CLOTH::SelfCollisionTest()
{
		if(SelfCollide && Simulate)
		{
			for(int j = 0; j < VertCount; j++)
			{
				for(int i = (j+1); i < VertCount; i++) //don't test <j against i
				{
					FLOAT3 centerTocenter = particles[i].position - particles[j].position; //i <-- j
					float centerTocenter_l = centerTocenter.Length();
					float rad_l = vertices[i].Radius + vertices[j].Radius;
					if (centerTocenter_l < rad_l)
					{
						//don't wanna move j...only move i
						float difference = (rad_l-centerTocenter_l);
						if(difference < 0)
							difference*=-1;
						particles[i].MovePosition(centerTocenter.Normalize()*difference); 		
					}
				}
				//check for emergency stop
				if(KEY_DOWN(VK_BACK))
				{
					Simulate = false;
					return false;
				}
			}
		}
	return true;
}
//=============================================================
//Check mouse click against collision spheres
//=============================================================
void CLOTH::MousePickingTest()
{
	if(DrawMesh && DrawVerts)
	{
		int indexChosen = -1;
		float bestDistance = 1000;

		for(int i = 0; i < VertCount; i++)
		{
			//get mesh
			LPD3DXMESH meshToTest = vertices[i].GetMesh();

			//create world matrix for mesh
			D3DXMATRIX world = worldMatrix;
			world._41 = vertices[i].GetCenter().x;
			world._42 = vertices[i].GetCenter().y;
			world._43 = vertices[i].GetCenter().z;

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
				///if first test or if better than a previous test distance wise...
				if((indexChosen == -1) || (distanceToCollision < bestDistance))
				{
					indexChosen = i;
					bestDistance = distanceToCollision;
				}
			}
		}
		//finished tests, update for click
		if(indexChosen != -1)
		{
			if(particles[indexChosen].IsPinned())
			{
				particles[indexChosen].PinParticle(false);
				if(Manipulate)
				{
					bool found = false;
					for(unsigned int i = 0; i < selected.size();i++)
					{
						if(selected[i] == indexChosen)
							vertices[indexChosen].SetBoundColor(D3DXVECTOR3(0,0,1.0f));		
						else
							vertices[indexChosen].SetBoundColor(D3DXVECTOR3(1.0f,0,0));		
					}
				}
				else
					vertices[indexChosen].SetBoundColor(D3DXVECTOR3(0,0,1.0f));
			}
			else
			{
				particles[indexChosen].PinParticle(true);
				vertices[indexChosen].SetBoundColor(D3DXVECTOR3(1.0f,0,0));
			}
		}
	}
}
//=============================================================
//Move pinned rows
//=============================================================
void CLOTH::MovePinnedRowUp(float speed)
{
	FLOAT3 direction(0,speed,0);
	for(unsigned int i = 0; i < selected.size(); i++)
		particles[selected[i]].AddForce(direction);
}
void CLOTH::MovePinnedRowRight(float speed)
{
	FLOAT3 direction(speed,0,0);
	for(unsigned int i = 0; i < selected.size(); i++)
		particles[selected[i]].AddForce(direction);
}
void CLOTH::MovePinnedRowForward(float speed)
{
	FLOAT3 direction(0,0,speed);
	for(unsigned int i = 0; i < selected.size(); i++)
		particles[selected[i]].AddForce(direction);
}
//=============================================================
//Select a row
//=============================================================
void CLOTH::SelectRow(int row)
{
	DeselectRow();
	SelectedRow = row;
	if(row == 1)
	{
		for(int i = 0; i < VertWidth; i++)
			selected.push_back(i);
	}
	else if(row == 2)
	{
		for(int i = 0; i < VertWidth; i++)
			selected.push_back((VertWidth*i) + (VertWidth-1));
	}
	else if(row == 3)
	{
		int r = VertWidth*(VertWidth-1);
		for(int i = 0; i < VertWidth; i++)
			selected.push_back(r+i);
	}
	else //row == 4
	{
		for(int i = 0; i < VertWidth; i++)
				selected.push_back(VertWidth*i);
	}
	//set bound color
	for(unsigned int i = 0; i < selected.size(); i++)
	{
		if(!particles[selected[i]].IsPinned())
			vertices[selected[i]].SetBoundColor(D3DXVECTOR3(0,1.0f,0));
	}
}
//=============================================================
//Select a row
//=============================================================
void CLOTH::DeselectRow()
{
	for(unsigned int i = 0; i < selected.size(); i++)
	{
		if(!particles[selected[i]].IsPinned())
			vertices[selected[i]].SetBoundColor(D3DXVECTOR3(0,0,1.0f));
	}
	selected.clear();
}
//=============================================================
//load texture
//=============================================================
void CLOTH::AddTexture(char * texDiffuse)
{	
	if(FAILED(D3DXCreateTextureFromFile(d3ddev, TEXT(texDiffuse), &m_texture)))
		exit(EXIT_FAILURE);
}