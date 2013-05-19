
#include "global.h"

SHADER SHADOWMAP::depthShader;
D3DXMATRIX SHADOWMAP::matShadowProj;
LPDIRECT3DSURFACE9 SHADOWMAP::ShadowZBuffer = NULL;

//=============================================================
//Constructor
//=============================================================
SHADOWMAP::SHADOWMAP()
{
	ShadowTexture = NULL;
	ShadowSurface = NULL;
}

//=============================================================
//Destructor
//=============================================================
SHADOWMAP::~SHADOWMAP()
{
	if(ShadowTexture != NULL)
		ShadowTexture->Release();
	if(ShadowSurface != NULL)
		ShadowSurface->Release();
	if(ShadowZBuffer != NULL)
	{
		ShadowZBuffer->Release();
		ShadowZBuffer = NULL;
	}
}

//=============================================================
//Lost Device
//=============================================================
void SHADOWMAP::OnLostDevice()
{
	depthShader.OnLostDevice();

	if(ShadowTexture != NULL)
		ShadowTexture->Release();
	if(ShadowSurface != NULL)
		ShadowSurface->Release();

	//do only once
	if(ShadowZBuffer != NULL)
	{
		ShadowZBuffer->Release();
		ShadowZBuffer = NULL;
		depthShader.OnLostDevice();
	}
}

//=============================================================
//Reset Device
//=============================================================
void SHADOWMAP::OnResetDevice()
{
	this->InitialiseShadowTexture();

	//do only once
	if(ShadowZBuffer == NULL)
	{
		depthShader.OnResetDevice();
		if(FAILED(d3ddev->CreateDepthStencilSurface(SHADOWMAP_SIZE, SHADOWMAP_SIZE, SHADOWBUFFER_FORMAT, 
													D3DMULTISAMPLE_NONE, NULL, TRUE, &ShadowZBuffer, NULL)))
		{
			MessageBox(NULL, TEXT("shadow Z-buffer creation has failed"), TEXT("ERROR"), MB_OK);
			exit(EXIT_FAILURE);
		}
	}
}

//=============================================================
//Initialise the texture and surface
//=============================================================
void SHADOWMAP::InitialiseShadowTexture()
{

	//Create the texture
	if(FAILED(D3DXCreateTexture(d3ddev, SHADOWMAP_SIZE, SHADOWMAP_SIZE, 1, D3DUSAGE_RENDERTARGET, 
								SHADOWMAP_FORMAT, D3DPOOL_DEFAULT, &ShadowTexture)))
	{
		MessageBox(NULL, TEXT("Shadow Texture Creation failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}
	ShadowTexture->GetSurfaceLevel(0, &ShadowSurface);

}

//=============================================================
//Initialise static shadowmap variables
//=============================================================
void SHADOWMAP::InitialiseShadowMapping()
{

	SHADOWMAP::CreateProjMatrix();
	depthShader.OnLoad("depthRender.fx");

   	//CREATE THE Z-BUFFER
    if(FAILED(d3ddev->CreateDepthStencilSurface(SHADOWMAP_SIZE, SHADOWMAP_SIZE, SHADOWBUFFER_FORMAT, 
												D3DMULTISAMPLE_NONE, NULL, TRUE, &ShadowZBuffer, NULL)))
	{
		MessageBox(NULL, TEXT("shadow Z-buffer creation has failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}

}

//=============================================================
//Create shadow projection matrix
//=============================================================
void SHADOWMAP::CreateProjMatrix()
{
	//projection matrix
	D3DXMatrixPerspectiveFovLH(&matShadowProj,
                               LightFov, //horizontal field of view
                               1.0f,
                               1.0f, //the near view-plane
                               1000.0f); //the far view-plane

}

//=============================================================
//Create shadow view matrix
//=============================================================
void SHADOWMAP::CreateViewMatrix(D3DXVECTOR3 LightPos, D3DXVECTOR3 LightForward, D3DXVECTOR3 LightUp)
{
	//create view matrix
    D3DXMatrixLookAtLH(&matShadowView, &LightPos, &LightForward, &LightUp);

	//create view proj matrix
	matShadowViewProj = matShadowView * matShadowProj;
}

//=============================================================
//Render the shadow map
//=============================================================
void SHADOWMAP::RenderShadowMap(D3DXVECTOR3 lightpos)
{
	gwd.matCurrentProjection = matShadowProj;
	gwd.matCurrentView = matShadowView;

	//set z-buffer
	if(FAILED(d3ddev->SetDepthStencilSurface(ShadowZBuffer)))
	{
		MessageBox(NULL, TEXT("Z-buffer setting has failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}

	//set render target
	d3ddev->SetRenderTarget(0, ShadowSurface);
	ClearRenderBuffer();

	//render all meshes with depth shader
	SHADER::TurnOnWorldShader();
	SHADER::SetWorldShader(depthShader.GetShader());
		Game.OnDrawMesh();
	SHADER::TurnOffWorldShader();

	//reset z-buffer
	if(FAILED(d3ddev->SetDepthStencilSurface(z_buffer)))
	{
		MessageBox(NULL, TEXT("Z-buffer resetting has failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}

	//set the projection/view matrixes to normal
	gwd.matCurrentView = Camera->matView;
	gwd.matCurrentProjection = Camera->matProjection;
}

//=============================================================
//Send shadow map to current shader
//=============================================================
void SHADOWMAP::SendShadowMap(LPD3DXEFFECT shader)
{
	shader->SetTexture("texShadow", ShadowTexture);
	shader->SetMatrix("LightViewProj", &matShadowViewProj);
}
