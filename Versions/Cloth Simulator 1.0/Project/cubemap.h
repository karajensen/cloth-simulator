/********************************************************************
 File: cubemap.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: Cubemap class; code based on MSN DirectX Cubemapping tutorial
 *******************************************************************/


#ifndef CUBEMAP_H_
#define CUBEMAP_H_

#include "includes.h"

#define FRAMES_BEFORE_UPDATING 8

class CUBEMAP
{
private:
	LPDIRECT3DCUBETEXTURE9 CubeMap; 
	int count;

	void CreateViewForCubeFace(DWORD faceNo, D3DXMATRIX world);
	void StartRenderCubeFace(DWORD faceNo);

public:
	static D3DXMATRIX matCubeProjection;
	static D3DXMATRIX matCubeView;
	static void InitialiseCubeMapping();

	~CUBEMAP();
	void CreateCubeMap();
	void RenderCubeMap(D3DXMATRIX world, bool & DrawMesh);
	LPDIRECT3DCUBETEXTURE9 & GetCubeMap(){ return CubeMap; };

};

#endif