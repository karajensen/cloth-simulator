/*
 *******************************************************************
 File: Game.h
 Author: Kara Jensen                   
 Date Created: 5/3/10               
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments:	Container for all objects in the game
 *******************************************************************
 */

#ifndef GAME_H_
#define GAME_H_

#include "shader.h"
#include "mesh.h"
#include "light.h"
#include "collision.h"
#include "sprite.h"
#include "text.h"
#include "cloth.h"

class GAME
{
private:

	vector<DTEXT> text;
	vector<SHADER> shaders;
	vector<SPRITE> sprites;
	vector<LIGHT> lights;
	vector<MESH> meshes;

	//Text
	stringstream s;
	string timestep;
	string vertcount;
	string damp;
	string size;
	string iterations;

	void InitialiseMeshes();
	void InitialiseShaders();
	void InitialiseLights();
	void InitialiseSprites();
	void InitialiseText();

public:

	void OnTimeStep();
	void InitialiseGame();
	void ReleaseGame();

	//text
	void OnDrawText();

	//sprites
	void OnDrawSprite();
	bool OnClickSprite();
	void SpriteClickEvents(int index);

	//meshes
	void OnDrawMesh();
	void OnClickMesh();

	//lights
	void SendAllLightsToShader(LPD3DXEFFECT shader);

	//collision
	bool CameraCollision(COLLISION_SPHERE * CameraSphere);
	bool CameraCollision(COLLISION_BOX * CameraBox);
	void MeshCollision();
	
	//Cloth
	CLOTH * cloth;
	float clothSize;
	int clothDimensions;
	void CreateNewCloth();
	void UpdateText();
	bool HandleMode;
	float HandleSpeed;
	bool MoveBall;

	//accessing
	DTEXT * GetText(int index){return &text[index];};
	SHADER * GetShader(int index){return &shaders[index];};
	SPRITE * GetSprite(int index){return &sprites[index];};
	MESH * GetMesh(int index){return &meshes[index];};
	LIGHT * GetLight(int index){return &lights[index];};
};

#endif