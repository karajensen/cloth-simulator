/********************************************************************
 File: rendertarget.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: Render Target References: http://www.borgsoft.de/renderToSurface.html
					    			 http://www.gamedev.net/community/forums/topic.asp?topic_id=316370
 *******************************************************************/

#ifndef RENDERTARGET_H_
#define RENDERTARGET_H_

#include "includes.h"
#include "shader.h"

class RENDERTARGET
{
private:
	LPDIRECT3DTEXTURE9 RenTarTexture;
	LPDIRECT3DSURFACE9 RenTarSurface;
	int postIndex;
	
	static LPDIRECT3DSURFACE9 AARenTarSurface;
	static LPDIRECT3DSURFACE9 BackBuffer;
	static vector<SHADER> postFX;

public:
	RENDERTARGET();
	~RENDERTARGET();
	void OnResetDevice();
	void OnLostDevice();
	LPDIRECT3DTEXTURE9 GetTexture(){ return RenTarTexture; };

	void CreateRenderTexture(UINT width, UINT height);
	void SaveRenderTargetToTexture();
	void RenderTexture();
	void SetIndex(int i){ postIndex = i; };

	static void SetAATarget();
	static void LoadPostFX(int index, char * file);
	static void RestoreDefaultBuffer();
	static void CreateRenderTarget();
};


#endif