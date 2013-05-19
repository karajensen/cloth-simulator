///////////////////////////////////////////////////////////////////////////////////
//RENDERING
///////////////////////////////////////////////////////////////////////////////////

#include "includes.h"
#include "global.h"

//=============================================================
//GAME RENDER
//=============================================================
void Render()
{
	StartRender();
	ClearRenderBuffer();
	gwd.matCurrentView = Camera->matView;
	gwd.matCurrentProjection = Camera->matProjection;

	//Draw meshes
	Game.OnDrawMesh();	

	//Draw sprites
	SPRITE::StartSpriteRender();
	Game.OnDrawSprite();
	SPRITE::EndSpriteRender();

	//Draw text
	Game.OnDrawText();

	EndRender();
}