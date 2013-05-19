#include "rendertarget.h"
#include "global.h" //for d3ddev

LPDIRECT3DSURFACE9 RENDERTARGET::BackBuffer = NULL;
LPDIRECT3DSURFACE9 RENDERTARGET::AARenTarSurface = NULL;
vector<SHADER> RENDERTARGET::postFX;

//=============================================================
//constructor
//=============================================================
RENDERTARGET::RENDERTARGET()
{
	RenTarTexture = NULL;
	RenTarSurface = NULL;
	postIndex = -1;
}

//=============================================================
//Destructor
//=============================================================
RENDERTARGET::~RENDERTARGET()
{
	if(RenTarTexture != NULL)
		RenTarTexture->Release();
	if(RenTarSurface != NULL)
		RenTarSurface->Release();
	if(AARenTarSurface != NULL)
	{
		AARenTarSurface->Release();
		AARenTarSurface = NULL;
	}
	if(BackBuffer != NULL)
	{
		BackBuffer->Release();
		BackBuffer = NULL;
	}
}

//=============================================================
//On Reset device
//=============================================================
void RENDERTARGET::OnResetDevice()
{
	this->CreateRenderTexture(gwd.WINDOW_WIDTH, gwd.WINDOW_HEIGHT);
	
	//Only do once
	if(BackBuffer == NULL)
	{	
		RENDERTARGET::CreateRenderTarget();
		for(int i = 0; i < MAX_POSTSHADERS; i++)
			postFX[i].OnResetDevice();
	}

}

//=============================================================
//On lost device
//=============================================================
void RENDERTARGET::OnLostDevice()
{
	if(RenTarTexture != NULL)
		RenTarTexture->Release();
	if(RenTarSurface != NULL)
		RenTarSurface->Release();
	
	//Only do once
	if(BackBuffer != NULL)
	{
		BackBuffer->Release();
		BackBuffer = NULL;
		AARenTarSurface->Release();
		AARenTarSurface = NULL;

		for(int i = 0; i < MAX_POSTSHADERS; i++)
			postFX[i].OnLostDevice();
	}
}

//=============================================================
//Create the AA Render target
//=============================================================
void RENDERTARGET::CreateRenderTarget()
{
	//save the backbuffer
	D3DSURFACE_DESC desc;
	d3ddev->GetRenderTarget(0, &BackBuffer);
	BackBuffer->GetDesc(&desc);
   
	//Create AA render target
	if(FAILED(d3ddev->CreateRenderTarget(gwd.WINDOW_WIDTH, gwd.WINDOW_HEIGHT, TEXTURE_FORMAT, 
										desc.MultiSampleType, desc.MultiSampleQuality,
										false, &AARenTarSurface, NULL)))
	 {
		MessageBox(NULL, TEXT("CreateRenderTarget failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}

	//create post shaders
	for(int i = 0; i < MAX_POSTSHADERS; i++)
	{
		SHADER temp;
		postFX.push_back(temp);
	}

}

//=============================================================
//Create the Texture to render to
//=============================================================
void RENDERTARGET::CreateRenderTexture(UINT width, UINT height)
{

	//Create the texture
	if(FAILED(D3DXCreateTexture(d3ddev, width, height, 1, D3DUSAGE_RENDERTARGET, 
								TEXTURE_FORMAT, D3DPOOL_DEFAULT, &RenTarTexture)))
	{
		MessageBox(NULL, TEXT("Dynamic Texture Creation failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}

	//save surface
	RenTarTexture->GetSurfaceLevel(0, &RenTarSurface);

}

//=============================================================
//Copy the render target surface to the texture
//=============================================================
void RENDERTARGET::SaveRenderTargetToTexture()
{
	if(FAILED(d3ddev->StretchRect(AARenTarSurface, NULL, RenTarSurface, NULL, D3DTEXF_LINEAR)))
	{
		MessageBox(NULL, TEXT("Stretch rect failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}
}

//=============================================================
//render environment texture created onto a sprite
//=============================================================
void RENDERTARGET::RenderTexture()
{
	D3DXVECTOR3 position(0, 0, 0); //position of sprite
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f); //center at the upper-left corner

	//LPDIRECT3DTEXTURE9 texture = Game.GetShadowMap();
	LPDIRECT3DTEXTURE9 texture = RenTarTexture;

	if(postIndex > 0)
	{
		LPD3DXEFFECT tempFX = postFX[postIndex].GetShader();		

		//SPRITE MATRIX: www.gamedev.net/community/forums/topic.asp?topic_id=525531
		D3DXMATRIX wvp;
		float width = float(gwd.WINDOW_WIDTH + 0.5);
		float height = float(gwd.WINDOW_HEIGHT + 0.5);
		D3DXMatrixOrthoOffCenterLH(&wvp,0.5f,width,height,0.5f,0.0f,1.0f);
		
		tempFX->SetMatrix("WorldViewProjection", &wvp);
		//tempFX->SetTexture("texColor",RenTarTexture); //set sprite texture (May not need)

		tempFX->Begin(&SHADER::totalPasses, 0);
		for(UINT pass = 0; pass < SHADER::totalPasses; pass++)
		{
			tempFX->BeginPass(pass);
			SPRITE::d3dspt->Draw(texture, NULL, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
			SPRITE::d3dspt->Flush();
			tempFX->EndPass(); 
		}
		tempFX->End();
	}
	else
		SPRITE::d3dspt->Draw(texture, NULL, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
	
}

//=============================================================
//Start Render To Target rendering
//=============================================================
void RENDERTARGET::SetAATarget()
{
	d3ddev->SetRenderTarget(0, AARenTarSurface);
}

//=============================================================
//Stop Render To Target rendering
//=============================================================
void RENDERTARGET::RestoreDefaultBuffer()
{
	d3ddev->SetRenderTarget(0,BackBuffer);
}

//=============================================================
//Load post effects
//=============================================================
void RENDERTARGET::LoadPostFX(int index, char * file)
{
	if((index >= 0) && (index < MAX_POSTSHADERS))
		postFX[index].OnLoad(file);
}