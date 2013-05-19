/********************************************************************
 File: shader.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: Shader class
 *******************************************************************/

#ifndef SHADER_H_
#define SHADER_H_


#include "includes.h"

class SHADER
{
private:
	LPD3DXEFFECT Shader;
	bool HasEnvironTechnique;

	static LPD3DXEFFECT currentShader;
	static bool usingWorldShader;

public:
	SHADER(){Shader = NULL;};
	~SHADER();
	void OnLoad(const char * filename);
	LPD3DXEFFECT GetShader(){ return Shader; };

	static UINT totalPasses;
	static LPD3DXEFFECT GetWorldShader(){ return currentShader; };
	static bool UseWorldShader(){ return usingWorldShader; };
	static void TurnOnWorldShader(){ usingWorldShader = true; };
	static void TurnOffWorldShader(){ usingWorldShader = false; };
	static void SetWorldShader(LPD3DXEFFECT shader){ currentShader = shader; };
	
};
#endif