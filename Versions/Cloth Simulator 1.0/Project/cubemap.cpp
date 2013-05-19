#include "cubemap.h"
#include "global.h" //for d3ddev

D3DXMATRIX CUBEMAP::matCubeProjection;
D3DXMATRIX CUBEMAP::matCubeView;

//=============================================================
//Destructor
//=============================================================
CUBEMAP::~CUBEMAP()
{
	CubeMap->Release();
}

//=============================================================
//Create the cube map
//=============================================================
void CUBEMAP::CreateCubeMap()
{
	//create counter (so all cube maps are not rendered at the same time)
	static int c_count = 0;
	count = c_count;
	c_count++;

	if(c_count > 7)
		c_count = 0;

   //Create the cube texture 
   if(FAILED(d3ddev->CreateCubeTexture(1024, 1, D3DUSAGE_RENDERTARGET, TEXTURE_FORMAT, D3DPOOL_DEFAULT, &CubeMap, NULL)))
   {
		MessageBox(NULL, TEXT("Cube Texture Creation failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
   }
}

//=============================================================
//Initialise the projection matrix
//=============================================================
void CUBEMAP::InitialiseCubeMapping()
{
    D3DXMatrixPerspectiveFovLH(&matCubeProjection,
                               D3DX_PI/2, //horizontal field of view
                               1.0f,
                               10.0f, //the near view-plane
                               1000.0f); //the far view-plane
}

//=============================================================
//Create View matrix for each face
//=============================================================
void CUBEMAP::CreateViewForCubeFace(DWORD faceNo, D3DXMATRIX world)
{
	D3DXVECTOR3 vEnvEyePt;
	vEnvEyePt.x = world._41;
	vEnvEyePt.y = world._42; 
	vEnvEyePt.z = world._43;

	D3DXVECTOR3 vLookatPt, vUpVec; //axis from center

		switch(faceNo) 
        {
            case D3DCUBEMAP_FACE_POSITIVE_X:
                vLookatPt = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
                vUpVec    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
                break;
            case D3DCUBEMAP_FACE_NEGATIVE_X:
                vLookatPt = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
                vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);
                break;
            case D3DCUBEMAP_FACE_POSITIVE_Y:
                vLookatPt = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
                vUpVec    = D3DXVECTOR3(0.0f, 0.0f,-1.0f);
                break;
            case D3DCUBEMAP_FACE_NEGATIVE_Y:
                vLookatPt = D3DXVECTOR3(0.0f,-1.0f, 0.0f);
                vUpVec    = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
                break;
            case D3DCUBEMAP_FACE_POSITIVE_Z:
                vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 1.0f);
                vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);
                break;
            case D3DCUBEMAP_FACE_NEGATIVE_Z:
                vLookatPt = D3DXVECTOR3(0.0f, 0.0f,-1.0f);
                vUpVec    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
                break;
        }

	//create new view matrix using axis
	D3DXMatrixLookAtLH(&matCubeView, &vEnvEyePt, &vLookatPt, &vUpVec);

}


//=============================================================
//Render the faces of a cube map
//=============================================================
void CUBEMAP::StartRenderCubeFace(DWORD faceNo)
{
	LPDIRECT3DSURFACE9 pFace;
	CubeMap->GetCubeMapSurface((D3DCUBEMAP_FACES)faceNo, 0, &pFace);
	d3ddev->SetRenderTarget(0,pFace);
	pFace->Release();
}


//=============================================================
//Render the faces of a cube map
//=============================================================
void CUBEMAP::RenderCubeMap(D3DXMATRIX world, bool & DrawMesh)
{
	count++;

	if(count >= FRAMES_BEFORE_UPDATING)
	{
		//for each face
		for (DWORD i = 0; i < 6; i++)
		{	
				//set the cube maps view matrix
				this->CreateViewForCubeFace(i, world);
				gwd.matCurrentView = CUBEMAP::matCubeView;
		
				//start the face render
				this->StartRenderCubeFace(i);
				ClearRenderBuffer();
	
				//RENDER SCENE:

				//set the mesh obtaining a cube map for to not be rendered
				DrawMesh = false;

				//render any objects to be included in reflections
				Game.OnDrawMesh();

				//set the mesh obtaining a cube map for to be rendered
				DrawMesh = true;
		}
		
		count = 0;
		
		//set the projection/view matrixes to normal
		gwd.matCurrentView = Camera->matView;
		gwd.matCurrentProjection = Camera->matProjection;
	}
}

