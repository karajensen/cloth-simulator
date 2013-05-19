/********************************************************************
 File: light.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: Light class
 *******************************************************************/

#ifndef LIGHT_H_
#define LIGHT_H_


#include "includes.h"
#include "transform.h"
#include "mesh.h"
#define LightFov (D3DX_PI / 2.0f)

class LIGHT : public TRANSFORM
{
private:
	D3DLIGHT9 light;
	float AmbientIntensity;
	float DiffuseIntensity;
	float SpecularIntensity;
	float SpecularSize;
	float LightOn;
public:

	LIGHT(){};
	~LIGHT();

	//light
	void InitialiseLight(bool lightOn);
	void SetAmbient(int r, int g, int b, float intensity);
	void SetDiffuse(int r, int g, int b, float intensity);
	void SetSpecular(int r, int g, int b, float intensity, float size);
	void SetAttentuation(float a0, float a1, float a2);
	void SendLightToShader(LPD3DXEFFECT shader);
	void LightSwitch(bool lightOn);

	//movement
	virtual void Translate(float x, float y, float z);
	virtual void Rotate(float x, float y, float z);
	virtual void MoveToPosition(float x, float y, float z);

protected:
	virtual void Scale(){}; //disable scaling
	virtual void Transform(){}; //disable scaling
};
#endif
