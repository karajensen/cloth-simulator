
#include "global.h"

UINT SHADER::totalPasses = 0; 
LPD3DXEFFECT SHADER::currentShader = NULL;
bool SHADER::usingWorldShader = false;

//=============================================================
//Destructor
//=============================================================
SHADER::~SHADER()
{
	if(Shader != NULL)
		Shader->Release();
}
//=============================================================
//Load Shader
//=============================================================
void SHADER::OnLoad(const char * filename)
{
	ID3DXBuffer* errorlog = NULL; //error buffer
    if(FAILED(D3DXCreateEffectFromFile(d3ddev, TEXT(filename),0,0,D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,0, &Shader, &errorlog)))
	{
		if (errorlog)//if there is an error
			MessageBox(NULL, (char*)errorlog->GetBufferPointer(), TEXT("EFFECT ERRORS"), MB_OK);
		else
		{
			string sfile = filename;
			sfile += " has failed";
			MessageBox(NULL, TEXT(ConvertSTR(sfile)), TEXT("ERROR"), MB_OK);
		}
		exit(EXIT_FAILURE);
	}
}
