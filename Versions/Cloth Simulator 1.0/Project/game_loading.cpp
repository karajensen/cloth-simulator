
#include "game.h"
#include "global.h"

//=============================================================================
//INITIALISE GAME
//=============================================================================
void GAME::InitialiseGame()
{
	SPRITE::CreateSpriteObject(); 
	COLLISION::InitialiseCollisions();
	COLLISION::DrawCollisions = true;
	HandleMode = false;
	HandleSpeed = 0.5f;

	for(int i = 0; i < MAX_SHADERS; i++)
		shaders.push_back(SHADER());
	for(int i = 0; i < MAX_LIGHTS; i++)
		lights.push_back(LIGHT());
	for(int i = 0; i < MAX_MESH; i++)
		meshes.push_back(MESH());
	for(int i = 0; i < MAX_SPRITE; i++)
		sprites.push_back(SPRITE());
	for(int i = 0; i < MAX_TEXT; i++)
		text.push_back(DTEXT());

	this->InitialiseLights();
	this->InitialiseShaders();
	this->InitialiseMeshes();
	this->InitialiseSprites();
	this->InitialiseText();
}

//=============================================================================
//INITIALISE LIGHTS
//=============================================================================
void GAME::InitialiseLights()
{
	lights[lMAIN].InitialiseLight(true);
	lights[lMAIN].MoveToPosition(-10,5,-18);
	lights[lMAIN].SetAmbient(240,240,255,0.05f);
	lights[lMAIN].SetDiffuse(240,240,255,0.7f);
	lights[lMAIN].SetSpecular(255,255,255,1.0f,14.0f);
	//lights[lMAIN].SetAttentuation(0,0,0);
}

//=============================================================================
//INITIALISE TEXT
//=============================================================================
void GAME::InitialiseText()
{
	int xposLeft = gwd.WINDOW_WIDTH-200;
	int xposRight = gwd.WINDOW_WIDTH-10;

	text[wTITLE].OnLoad("Tahoma", false, 1000, 16, xposLeft, 10, xposRight, 30);
	text[wTITLE].SetText("SIT352: Cloth Simulator");
	text[wTITLE].SetColour(255/2,255,255,255);
	text[wTITLE].SetAlign(alignRIGHT);

	s.precision(1);
	s.setf(ios_base::floatfield, ios_base::fixed);
	s.setf(ios_base::showpoint);

	s << TIMESTEP;
	timestep = s.str() + " [TIME]" + "\n";
	s.seekp(0);
	s.str("");

	s << cloth->getVertCount();
	vertcount = s.str() + " [VERT]" + "\n";
	s.seekp(0);
	s.str("");

	s << DAMPING;
	damp = s.str() + " [DAMP]" + "\n";
	s.seekp(0);
	s.str("");

	s << clothSize;
	size = s.str() + " [SIZE]" + "\n";
	s.seekp(0);
	s.str("");

	s << cloth->GetSpringIterations();
	iterations = s.str() + " [ITRN]" + "\n";
	s.seekp(0);
	s.str("");

	text[wSTATS].OnLoad("Tahoma", false, 400, 15, xposLeft, 30, xposRight-36, 60);
	text[wSTATS].SetText(timestep + damp + vertcount + size + iterations);
	text[wSTATS].SetColour(255,255,255,255);
	text[wSTATS].SetAlign(alignRIGHT);

	float i = 29.0f; float h = 15.0f;
	sprites[iPLUStime].OnLoad(".\\Resources\\Sprites\\plus.png",255,16,16,float(xposRight)-33,(i),16,16);
	sprites[iMINUStime].OnLoad(".\\Resources\\Sprites\\minus.png",255,16,16,float(xposRight)-16,(i),16,16);
	sprites[iPLUSdamp].OnLoad(".\\Resources\\Sprites\\plus.png",255,16,16,float(xposRight)-33,(i+h),16,16);
	sprites[iMINUSdamp].OnLoad(".\\Resources\\Sprites\\minus.png",255,16,16,float(xposRight)-16,(i+h),16,16);
	sprites[iPLUSvert].OnLoad(".\\Resources\\Sprites\\plus.png",255,16,16,float(xposRight)-33,(i+h+h),16,16);
	sprites[iMINUSvert].OnLoad(".\\Resources\\Sprites\\minus.png",255,16,16,float(xposRight)-16,(i+h+h),16,16);
	sprites[iPLUSsize].OnLoad(".\\Resources\\Sprites\\plus.png",255,16,16,float(xposRight)-33,(i+h+h+h),16,16);
	sprites[iMINUSsize].OnLoad(".\\Resources\\Sprites\\minus.png",255,16,16,float(xposRight)-16,(i+h+h+h),16,16);
	sprites[iPLUSit].OnLoad(".\\Resources\\Sprites\\plus.png",255,16,16,float(xposRight)-33,(i+h+h+h+h),16,16);
	sprites[iMINUSit].OnLoad(".\\Resources\\Sprites\\minus.png",255,16,16,float(xposRight)-16,(i+h+h+h+h),16,16);
}

//=============================================================================
//INITIALISE SPRITES
//=============================================================================
void GAME::InitialiseSprites()
{
	float Y = 20;
	float X = 20;
	sprites[iGRAVITY].OnLoad(".\\Resources\\Sprites\\gravity.png",(255/2),32,32,X,Y,32,32);
	sprites[iSHOWVERTS].OnLoad(".\\Resources\\Sprites\\showVerts.png",255,32,32,X,(Y+(32*1)-1),32,32);
	sprites[iBOX].OnLoad(".\\Resources\\Sprites\\box.png",(255),32,32,X,(Y+(32*2)-2),32,32);
	sprites[iMOVE].OnLoad(".\\Resources\\Sprites\\move.png",(255/2),32,32,X,(Y+(32*3)-3),32,32);
	sprites[iCAMRESET].OnLoad(".\\Resources\\Sprites\\resetCam.png",255,32,32,X,(Y+(32*4)-4),32,32);
	sprites[iCLOTHRESET].OnLoad(".\\Resources\\Sprites\\resetCloth.png",255,32,32,X,(Y+(32*5)-5),32,32);
	sprites[iGRAB].OnLoad(".\\Resources\\Sprites\\grab.png",(255),32,32,X,(Y+(32*6)-6),32,32);
	sprites[iHANDLE].OnLoad(".\\Resources\\Sprites\\handle.png",(255/2),32,32,X,(Y+(32*7)-7),32,32);
	sprites[iCOLLIDE].OnLoad(".\\Resources\\Sprites\\collide.png",(255/2),32,32,X,(Y+(32*8)-8),32,32);
	sprites[iKEYS].OnLoad(".\\Resources\\Sprites\\keys.png",(255/2),32,32,X,(Y+(32*9)-9),32,32);

	float xl = float(gwd.WINDOW_WIDTH-266);
	float yl = float(gwd.WINDOW_HEIGHT-266);
	sprites[iKEYLIST].OnLoad(".\\Resources\\Sprites\\keylist.png",0,256,256,xl,yl,256,256);

}
//=============================================================================
//INITIALISE SHADERS
//=============================================================================
void GAME::InitialiseShaders()
{
	shaders[xTEX].OnLoad(".\\Resources\\Shaders\\texture.fx");
	shaders[xMAIN].OnLoad(".\\Resources\\Shaders\\main.fx");
	shaders[xHIGHLIGHT].OnLoad(".\\Resources\\Shaders\\highlight.fx");
	shaders[xCLOTH].OnLoad(".\\Resources\\Shaders\\cloth.fx");
}

//=============================================================================
//INITIALISE MESHES
//=============================================================================
void GAME::InitialiseMeshes()
{
	cloth = new CLOTH();
	clothDimensions = 30;
	clothSize = 0.5;
	cloth->OnLoad(clothDimensions,clothDimensions,clothSize,&shaders[xCLOTH]);
	cloth->AddTexture(".\\Resources\\Models\\square.png");

	meshes[mBALL].OnLoad(L"ball.obj",L".\\Resources\\Models\\",&shaders[xMAIN],mBALL);
	meshes[mBALL].Scale(0.4f,0.4f,0.4f);
	meshes[mBALL].MoveToPosition(0,-2,0);
	meshes[mBALL].CreateBoundingSphere(2.5,meshes[mBALL].GetPosition(),false);
	MoveBall = false;

	meshes[mGROUND].OnLoad(L"ground.obj",L".\\Resources\\Models\\",&shaders[xMAIN],mGROUND);
	meshes[mGROUND].MoveToPosition(0,-20,0);
	meshes[mGROUND].CreateBoundingBox(150,1,150,meshes[mGROUND].GetPosition(),false);

}
