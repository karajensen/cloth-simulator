/********************************************************************
 File: shadowmap.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments:	class to create a shadow map. 
			Code: Derived from DirectX 9 Sample Browser "ShadowMap" by Microsoft
 *******************************************************************/

#ifndef SHADOWMAP_H_
#define SHADOWMAP_H_

#include "includes.h"
#include "shader.h"

#define SHADOWMAP_SIZE 1024
#define SHADOWBUFFER_FORMAT (D3DFMT_D24S8)
#define SHADOWMAP_FORMAT (D3DFMT_R32F)

class SHADOWMAP
{
private:
	LPDIRECT3DTEXTURE9 ShadowTexture;
	LPDIRECT3DSURFACE9 ShadowSurface;
	D3DXMATRIX matShadowView;
	D3DXMATRIX matShadowViewProj;

	static D3DXMATRIX matShadowProj;
	static SHADER depthShader;
	static LPDIRECT3DSURFACE9 ShadowZBuffer;
	static void CreateProjMatrix();

public:
	SHADOWMAP();
	~SHADOWMAP();
	void OnLostDevice();
	void OnResetDevice();
	LPDIRECT3DTEXTURE9 GetTexture(){ return ShadowTexture; };

	void InitialiseShadowTexture();
	void RenderShadowMap(D3DXVECTOR3 lightpos);
	void SendShadowMap(LPD3DXEFFECT shader);
	void CreateViewMatrix(D3DXVECTOR3 LightPos, D3DXVECTOR3 LightForward, D3DXVECTOR3 LightUp);

	static void InitialiseShadowMapping();
};


#endif
