

#include "global.h"
#include "mesh.h"
#include "GFXLoader.h"

//=============================================================
//Constructor
//=============================================================
MESH::MESH()
{
	m_texture = NULL;
	m_pMesh = NULL; 
	m_shader = NULL; 
	b_collision = NULL;
	s_collision = NULL;
	m_Technique = NULL;
}
//=============================================================
//Destructor
//=============================================================
MESH::~MESH()
{
	if(m_Loader.isLoaded()){ m_Loader.Release(); }
	if(m_texture != NULL){ m_texture->Release(); }

	SAFE_RELEASE(m_pMesh);
	if(b_collision != NULL)
	{
		b_collision->ReleaseCollision();
		delete b_collision;
	}
	if(s_collision != NULL)
	{
		s_collision->ReleaseCollision();
		delete s_collision;
	}
}
//=============================================================
//Load the mesh
//=============================================================
bool MESH::OnLoad(WCHAR const* wszFilename, WCHAR const* wszDirectory, SHADER * meshFX, int index, D3DXHANDLE technique)
{
	//CREATE GFXLOADER
	m_Loader.LoadGeometryFromOBJ(wszFilename,wszDirectory);

	//CREATE VERTEX DEFINITION
	D3DVERTEXELEMENT9 VERTEX_DECL[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },		
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	//LOAD MESH FROM GFXLOADER
	if( !m_Loader.isLoaded() )
		return FALSE;
	std::vector<VERTEX>&		vertexData = m_Loader.GetVertexData();
	std::vector<DWORD>&			indexData = m_Loader.GetIndexData();


	if( FAILED(	D3DXCreateMesh(indexData.size() / 3,
							   vertexData.size(),
							   D3DXMESH_MANAGED | D3DXMESH_32BIT,
							   VERTEX_DECL,
							   d3ddev,
							   &m_pMesh) ) )
	{
		MessageBox(NULL, TEXT("Mesh creation failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}

	LPVERTEX pVertexBuffer;
	if( FAILED(	m_pMesh->LockVertexBuffer(0, (void**)&pVertexBuffer) ) )
	{
		MessageBox(NULL, TEXT("Vertex buffer lock failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}
	#pragma warning(disable: 4996)
	std::copy(vertexData.begin(), vertexData.end(), pVertexBuffer);
	m_pMesh->UnlockVertexBuffer();


	DWORD* pIndexBuffer;
	if( FAILED( m_pMesh->LockIndexBuffer(0, (void**)&pIndexBuffer) ) )
	{
		MessageBox(NULL, TEXT("Index buffer lock failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}
	#pragma warning(disable: 4996)
	std::copy(indexData.begin(), indexData.end(), pIndexBuffer);
	m_pMesh->UnlockIndexBuffer();


	//SET VALUES
	DrawMesh = true;
	HasCollisions = false;
	Index = index;
	timer = NULL;
	randomNumber = NULL;
	MAX_RAND_RANGE = NULL;

	//SHADERS	
	m_shader = meshFX;
	LPD3DXEFFECT currentFX = meshFX->GetShader();
	m_Technique = technique;

	//FINISH
	return TRUE;
}

//=============================================================
//Recreate mesh with Bump normals
//=============================================================
void MESH::CreateNormalsForDetailMapping()
{
	//RECREATE MESH WITH BINOMALS/TANGENT
	//FOLLOWING CODE FROM: http://myfeldin.com/programming/parallaxMapping.php
	D3DVERTEXELEMENT9 vertDecl[] = 
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
		{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};

	//clonedMesh is where the new mesh with the new vertexDecl will be, and new mesh will be passed into the ComputeTangent function. Compute tangent
	//might modify some vertices to get correct tangent data so it needs this mesh.
	LPD3DXMESH clonedMesh, newMesh;

	//clone your mesh with new data and store it in clonedMesh
	m_pMesh->CloneMesh(D3DXMESH_VB_MANAGED, vertDecl, d3ddev, &clonedMesh);
	
	//release old mesh
	m_pMesh->Release();

	//And heres what you really need
	D3DXComputeTangentFrameEx( clonedMesh, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
                               D3DDECLUSAGE_BINORMAL, 0, D3DDECLUSAGE_NORMAL, 0,
                               D3DXTANGENT_CALCULATE_NORMALS,
							   NULL, -1, 0, -1, &newMesh, NULL );

	//don't need clonedMesh anymore
	clonedMesh->Release();
	
	//set new mesh to the one that was created
	m_pMesh     = newMesh;
	newMesh  = 0;
}

//=============================================================
//Create custom collision box
//=============================================================
void MESH::CreateBoundingBox(float Width, float Height, float Depth, D3DXVECTOR3 Location, bool hasMesh)
{
	if(!HasCollisions)
	{
		HasCollisions = true;
		b_collision = new COLLISION_BOX;
		b_collision->Initialise(Width,Height,Depth,Location,hasMesh);
	}
}

//=============================================================
//Create custom collision sphere
//=============================================================
void MESH::CreateBoundingSphere(float Radius, D3DXVECTOR3 Location, bool hasMesh)
{
	if(!HasCollisions)
	{
		HasCollisions = true;
		s_collision = new COLLISION_SPHERE;
		s_collision->Initialise(Radius,Location,hasMesh);
	}
}