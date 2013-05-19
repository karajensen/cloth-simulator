/********************************************************************
 File: Camera.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: Camera class; code based on Toymaker Camera class
 *******************************************************************/

#ifndef CAMERA_H_
#define CAMERA_H_


#include "includes.h"

class CAMERA
{
private:
	D3DXVECTOR3 initialPos;
	D3DXVECTOR3 pos; //Camera position in world space
	D3DXVECTOR3 up, forward, right; //Camera axis
	float yaw;  //Y axis rotation
	float pitch; //X axis rotation
	float roll; //Z axis rotation
	bool ViewNeedsUpdating;
	bool reset;
	bool MovedForward;
	bool MovedBackward;

public:
	D3DXMATRIX matWorld;
	D3DXMATRIX matProjection; //projection transform matrix
	D3DXMATRIX matView; //view transform matrix

	CAMERA(float x, float y, float z);
	D3DXVECTOR3 & GetPos(){return pos;};
	D3DXVECTOR3 & GetForwardVector(){return forward;};

	float GetYaw(){return yaw;};
	float GetPitch(){return pitch;};
	float GetRoll(){return roll;};
	void SetYaw(float y){yaw = y;};
	void SetPitch(float p){pitch = p;};
	void SetRoll(float r){roll = r;};
	
	void SetCameraNeedsUpdating(bool m){ViewNeedsUpdating = m;};
	void SetMovedForward(bool m){MovedForward = m;};
	void SetMovedBackward(bool m){MovedBackward = m;};
	bool CameraNeedsUpdating(){return ViewNeedsUpdating;};
	bool CameraHasBeenReset(){return reset;};
	void CameraNowReset(){reset = true;};
	bool HasMovedForward(){return MovedForward;};
	bool HasMovedBackward(){return MovedBackward;};
	void CreateViewMatrix();
	void CreateProjMatrix();
	void ResetCamera();


	//transforming Camera
	void Forward(float val);
	void Right(float val);
	void Up(float val);
	void Yaw(float angle);
	void Pitch(float angle);
	void Roll(float angle);

	//Camera speed
	float movSpeed; //movement speed
	float rotSpeed; //rotation speed
};

#endif