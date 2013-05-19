
#include "gui.h"
#include "global.h"

//=============================================================================
//Initialise GUI
//=============================================================================
void GUI::OnLoad()
{
	for(int i = 0; i < MAX_GUIANISPRITES; i++)
		//guiAnisprites.push_back(ANISPRITE());
	for(int i = 0; i < MAX_GUISPRITES; i++)
		//guiSprites.push_back(SPRITE());
	for(int i = 0; i < MAX_GUITEXT; i++)
		//guiText.push_back(DTEXT());

	clock.OnLoad("Tahoma",false,800,14,33,35,33+80,35+42);
	clock.SetAlign("left");
	clock.SetColour(255,201,21,21);
	clock.InitialiseClock(300,0,true);

}

//=============================================================================
//Draw GUI sprites
//=============================================================================
void GUI::OnDrawGUISprites()
{
	/*
	for(int i = 0; i < MAX_GUISPRITES; i++)
		guiSprites.OnDraw();

	for(int i = 0; i < MAX_GUIANISPRITES; i++)
		guiAnisprites.OnDraw();*/
}

//=============================================================================
//Draw GUI text
//=============================================================================
void GUI::OnDrawGUIText()
{
	for(int i = 0; i < MAX_GUITEXT; i++)
		//guiText.OnDraw();

	clock.OnDraw();
}

//=============================================================================
//Check if GUI was clicked
//=============================================================================
bool GUI::OnClickGUI()
{
	return false;
}

//=============================================================================
//animate GUI
//=============================================================================
void GUI::AnimateGUI()
{
}

//=============================================================================
//Set the text
//=============================================================================
void GUI::SetText(string t, int index)
{	
	//guiText[index].SetText(t); 
}
void GUI::SetText(char * t, int index)
{
	//guiText[index].SetText(t); 
}

//=============================================================================
//Lost device
//=============================================================================
void GUI::OnLostDevice()
{
	for(int i = 0; i < MAX_GUITEXT; i++)
		//guiText.OnLostDevice();

	clock.OnLostDevice();
}

//=============================================================================
//reset device
//=============================================================================
void GUI::OnResetDevice()
{
	for(int i = 0; i < MAX_GUITEXT; i++)
//		guiText.OnResetDevice();

	clock.OnResetDevice();
}

//=============================================================================
//Start all clocks ticking
//=============================================================================
void GUI::StartClocks()
{
	clock.StartClock();
}

//=============================================================================
//Calculate time for all clocks
//=============================================================================
void GUI::CalculateClocks()
{
	clock.CalculateTimePassed();
}

//=============================================================================
//Stop all clocks ticking
//=============================================================================
void GUI::StopClocks()
{
	clock.StopClock();
}

