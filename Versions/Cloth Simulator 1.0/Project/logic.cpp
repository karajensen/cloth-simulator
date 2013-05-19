///////////////////////////////////////////////////////////////////////////////////
//GAME LOGIC (determine course of action from input)
///////////////////////////////////////////////////////////////////////////////////


#include "includes.h"
#include "global.h"

//=============================================================
//LOGIC
//=============================================================
void Logic()
{		
	if(!Game.HandleMode)
	{
		//CAMERA
		if (InputData.Left)
			Camera->Right(-Camera->movSpeed);
		if (InputData.Right)
			Camera->Right(Camera->movSpeed);
		if (InputData.Up)
			Camera->Forward(Camera->movSpeed);
		if (InputData.Down)
			Camera->Forward(-Camera->movSpeed);
		if (InputData.kW)
			Camera->Up(Camera->movSpeed);
		if (InputData.kQ)
			Camera->Up(-Camera->movSpeed);
		if (InputData.kZ)
			Camera->Pitch(Camera->rotSpeed);
		if (InputData.kX)
			Camera->Pitch(-Camera->rotSpeed);
		if (InputData.kC)
			Camera->Yaw(Camera->rotSpeed);
		if (InputData.kV)
			Camera->Yaw(-Camera->rotSpeed);
		if (InputData.kA)
			Camera->Roll(Camera->rotSpeed);
		if (InputData.kS)
			Camera->Roll(-Camera->rotSpeed);
	}
	else
	{
		if (InputData.Left)
			Game.cloth->MovePinnedRowRight(-Game.HandleSpeed);
		if (InputData.Right)
			Game.cloth->MovePinnedRowRight(Game.HandleSpeed);
		if (InputData.Up)
			Game.cloth->MovePinnedRowForward(Game.HandleSpeed);
		if (InputData.Down)
			Game.cloth->MovePinnedRowForward(-Game.HandleSpeed);
		if (InputData.kW)
			Game.cloth->MovePinnedRowUp(Game.HandleSpeed);
		if (InputData.kQ)
			Game.cloth->MovePinnedRowUp(-Game.HandleSpeed);
		if (InputData.k1)
			Game.cloth->SelectRow(1);
		if (InputData.k2)
			Game.cloth->SelectRow(2);
		if (InputData.k3)
			Game.cloth->SelectRow(3);
		if (InputData.k4)
			Game.cloth->SelectRow(4);
	}

	if(Camera->CameraNeedsUpdating())
	{
		Camera->CreateViewMatrix();
	}
	else if(Camera->CameraHasBeenReset())
	{
		Camera->SetCameraNeedsUpdating(true);
		Camera->CreateViewMatrix();
		Camera->CameraNowReset();
	}

	//=================================================
	//MOUSE CLICK
	//=================================================
		if (InputData.LMB || InputData.RMB)
		{
			//if no sprites are clicked check meshes
			if(!Game.OnClickSprite())
			{
					Picking(); //get mouse ray
					Game.OnClickMesh();
			}
		}
	
	//=================================================
	//PHYSICS/COLLISIONS
	//=================================================
	Game.OnTimeStep();
	Game.MeshCollision();

	//=================================================
	//RESET INPUT
	//=================================================
	Camera->SetCameraNeedsUpdating(false);
	ResetInput();
}
//=============================================================
//RESET INPUT
//=============================================================
void ResetInput()
{
	ZeroMemory(&InputData, sizeof(INPUTDATA)); //reset input data each loop
	InputData.LMB = false;
	InputData.RMB = false;
	InputData.mX = -1;
	InputData.mY = -1;
	InputData.meshClicked = NULL;
}