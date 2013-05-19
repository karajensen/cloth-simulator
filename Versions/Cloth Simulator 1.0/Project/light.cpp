
#include "global.h"
#include "light.h"

//=============================================================
//DESTRUCTOR
//=============================================================
LIGHT::~LIGHT()
{
}

//=============================================================
//Initialise lights
//=============================================================
void LIGHT::InitialiseLight(bool lightOn)
{
	//turn light on
	if(lightOn)
		LightOn = 1.0f;
	else
		LightOn = 0;

	ZeroMemory(&light, sizeof(light));

	//set standard attentuation (directional light)
	light.Theta = LightFov / 2.0f;
	light.Phi = LightFov / 2.0f;
	light.Attenuation0 = 0;
	light.Attenuation1 = 1.0f;
	light.Attenuation2 = 0;

	//set standard values
	this->SetAmbient(255,255,255,0.1f);
	this->SetDiffuse(255,255,255,0.7f);
	this->SetSpecular(255,255,255,1.0f,14.0f);
}

//=============================================================
//Translate/rotate the light
//=============================================================
void LIGHT::Translate(float x, float y, float z)
{
	this->TRANSFORM::Translate(x,y,z);
}
void LIGHT::Rotate(float x, float y, float z)
{
	this->TRANSFORM::Rotate(x,y,z);
}
void LIGHT::MoveToPosition(float x, float y, float z)
{
	this->TRANSFORM::MoveToPosition(x,y,z);
}

//=============================================================
//Set the ambient portion of light
//=============================================================
void LIGHT::SetAmbient(int r, int g, int b, float intensity)
{
    light.Ambient.r = (float(r))/255.0f;
	light.Ambient.g = (float(g))/255.0f;
	light.Ambient.b = (float(b))/255.0f;
	AmbientIntensity = intensity;
}

//=============================================================
//Set the diffuse portion of light
//=============================================================
void LIGHT::SetDiffuse(int r, int g, int b, float intensity)
{
	light.Diffuse.r = (float(r))/255.0f;
	light.Diffuse.g = (float(g))/255.0f;
	light.Diffuse.b = (float(b))/255.0f;
	DiffuseIntensity = intensity;
}

//=============================================================
//Set the specular portion of light
//=============================================================
void LIGHT::SetSpecular(int r, int g, int b, float intensity, float size)
{
	light.Specular.r = (float(r))/255.0f;
	light.Specular.g = (float(g))/255.0f;
	light.Specular.b = (float(b))/255.0f;
	SpecularIntensity = intensity;
	SpecularSize = size;
}

//=============================================================
//Set the dropoff rate
//=============================================================
void LIGHT::SetAttentuation(float a0, float a1, float a2)
{
	light.Attenuation0 = a0;
	light.Attenuation1 = a1;
	light.Attenuation2 = a2;
}


//=============================================================
//Send lights to HLSL shader
//=============================================================
void LIGHT::SendLightToShader(LPD3DXEFFECT shader)
{
	shader->SetFloat("LightOn", LightOn);
	if(LightOn >= 1.0f)
	{
		shader->SetFloatArray("AmbientColor", &light.Ambient.r, 3);
		shader->SetFloatArray("DiffuseColor", &light.Diffuse.r, 3);
		shader->SetFloatArray("SpecularColor", &light.Specular.r, 3);
		shader->SetFloat("AmbientIntensity", AmbientIntensity);
		shader->SetFloat("DiffuseIntensity", DiffuseIntensity);
		shader->SetFloat("SpecularIntensity", SpecularIntensity);
		shader->SetFloat("SpecularSize", SpecularSize);
		shader->SetFloat("att0", light.Attenuation0);
		shader->SetFloat("att1", light.Attenuation1);
		shader->SetFloat("att2", light.Attenuation2);

		//workaround:
		float * pos = (float*)this->GetPosition();
		shader->SetFloatArray("LightPos", pos, 3); 
	}
}
