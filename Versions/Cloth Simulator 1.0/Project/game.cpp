
#include "global.h"
#include "game.h"

//=============================================================================
//RELEASE GAME
//=============================================================================
void GAME::ReleaseGame()
{
	cloth->ReleaseCloth();
	delete cloth;
	SPRITE::ReleaseSpriteObject();
}

//=============================================================================
//DRAW MESHES
//=============================================================================
void GAME::OnDrawMesh()
{
	cloth->OnDraw();

	for(int i = 0; i < MAX_MESH; i++)
		meshes[i].OnDraw();
}
//=============================================================================
//TIMESTEP
//=============================================================================
void GAME::OnTimeStep()
{
	if(MoveBall)
	{
		static float move;
		static float amount = 0.2f;
		move += amount;
		meshes[mBALL].MoveToPosition(0,-2,move);
		meshes[mBALL].GetCollisionSphere()->UpdateCollision(meshes[mBALL].GetPosition());
		if((move >=20) || (move <= -20))
			amount *= -1;
	}
	cloth->OnTimeStep();
}
//=============================================================================
//DRAW SPRITES
//=============================================================================
void GAME::OnDrawSprite()
{
	for(int i = 0; i < MAX_SPRITE; i++)
		sprites[i].OnDraw();
}

//=============================================================================
//DRAW TEXT
//=============================================================================
void GAME::OnDrawText()
{
	for(int i = 0; i < MAX_TEXT; i++)
		text[i].OnDraw();
}
//=============================================================================
//Send all lights to shader
//=============================================================================
void GAME::SendAllLightsToShader(LPD3DXEFFECT shader)
{
	for(int i = 0; i < MAX_LIGHTS; i++)
		lights[i].SendLightToShader(shader);
}

//=============================================================================
//Test Camera space (sphere) against meshes
//=============================================================================
bool GAME::CameraCollision(COLLISION_SPHERE * CameraSphere)
{
	//Test if Camera collide with world
	for(int i = 0; i < MAX_MESH; i++)
		if(meshes[i].CollisionTest(CameraSphere))
			return true;
	return false;
}

//=============================================================================
//Test Camera space (box) against meshes
//=============================================================================
bool GAME::CameraCollision(COLLISION_BOX * CameraBox)
{
	//Test if Camera collide with world
	for(int i = 0; i < MAX_MESH; i++)
		if(meshes[i].CollisionTest(CameraBox))
			return true;
	return false;
}
//=============================================================================
//Test mesh collisions
//=============================================================================
void GAME::MeshCollision()
{
	//Test cloth
	if(!cloth->SelfCollisionTest())
		sprites[iGRAVITY].SetVisibility(255/2);

	//objects to test against cloth
	if(meshes[mBALL].IsVisible())
		cloth->TestSphereCollision(meshes[mBALL].GetCollisionSphere());
	cloth->TestBoxCollision(meshes[mGROUND].GetCollisionBox());
}

//=============================================================================
//Check whether sprites have been clicked
//=============================================================================
bool GAME::OnClickSprite()
{
	int i;
	for(i = 0; i < MAX_SPRITE; i++)
	{
		if(sprites[i].OnClick())
		{
			SpriteClickEvents(i);
			return true;
		}
	}
	return false;
}
//=============================================================================
//Sprite click events
//=============================================================================
void GAME::SpriteClickEvents(int index)
{
	static const float INCREASE = float(0.1);
	switch(index)
	{
	case iGRAVITY:
	{	
		if(!HandleMode)
		{
			sprites[iGRAVITY].ToggleVisibility();
			cloth->ToggleSimulation();
		}
		break;
	}
	case iGRAB:
	{	
		cloth->UnpinCloth();
		break;
	}
	case iBOX:
	{	
		if(meshes[mBALL].IsVisible())
			meshes[mBALL].SetVisibility(false);
		else
			meshes[mBALL].SetVisibility(true);
		sprites[iBOX].ToggleVisibility();
		break;
	}
	case iCAMRESET:
	{	
		Camera->ResetCamera();
		break;
	}
	case iCLOTHRESET:
	{
		cloth->Reset();
		sprites[iGRAVITY].SetVisibility(255/2);
		break;
	}
	case iPLUStime: //timestep
	{
		TIMESTEP += INCREASE;
		TIMESTEP_SQUARED = TIMESTEP*TIMESTEP;
		s << TIMESTEP;
		timestep = s.str() + " [TIME]" + "\n";
		UpdateText();
		break;
	}
	case iMINUStime: //timestep
	{
		if(TIMESTEP >= 0.01)
		{
			TIMESTEP -= INCREASE;
			TIMESTEP_SQUARED = TIMESTEP*TIMESTEP;
			s << TIMESTEP;
			timestep = s.str() + " [TIME]" + "\n";
			UpdateText();
		}
		break;
	}
	case iPLUSdamp: //dampening
	{
		DAMPING += INCREASE;
		s << DAMPING;
		damp = s.str() + " [DAMP]" + "\n";
		UpdateText();
		break;
	}
	case iMINUSdamp: //dampening
	{
		if(DAMPING >= 0.01)
		{
			DAMPING -= INCREASE;
			s << DAMPING;
			damp = s.str() + " [DAMP]" + "\n";
			UpdateText();
		}
		break;
	}
	case iMINUSvert: 
	{
		if(clothDimensions > 2)
		{
			clothDimensions--;
			CreateNewCloth();
			s << cloth->getVertCount();
			vertcount = s.str() + " [VERT]" + "\n";
			UpdateText();
		}
		break;
	}
	case iPLUSvert:
	{
		clothDimensions++;
		CreateNewCloth();
		s << cloth->getVertCount();
		vertcount = s.str() + " [VERT]" + "\n";
		UpdateText();
		break;
	}
	case iPLUSsize:
	{
		clothSize+=INCREASE;
		CreateNewCloth();
		s << clothSize;
		size = s.str() + " [SIZE]" + "\n";
		UpdateText();
		break;
	}
	case iMINUSsize:
	{
		if(clothSize >= 0.11)
		{
			clothSize-=INCREASE;
			CreateNewCloth();
			s << clothSize;
			size = s.str() + " [SIZE]" + "\n";
			UpdateText();
		}
		break;
	}
	case iMINUSit:
	{
		int itr = cloth->GetSpringIterations();
		if(itr > 2)
		{
			cloth->SetSpringIterations(--itr);
			s << cloth->GetSpringIterations();
			iterations = s.str() + " [ITRN]" + "\n";
			UpdateText();
		}	
		break;
	}
	case iPLUSit:
	{
		int itr = cloth->GetSpringIterations();
		cloth->SetSpringIterations(++itr);
		s << cloth->GetSpringIterations();
		iterations = s.str() + " [ITRN]" + "\n";
		UpdateText();
		break;
	}
	case iSHOWVERTS:
	{
		cloth->ToggleShowVerts();
		sprites[iSHOWVERTS].ToggleVisibility();
		break;
	}
	case iHANDLE:
	{
		if(HandleMode == false)
		{
			cloth->SelectRow(cloth->SelectedRow);
			cloth->SetManipulate(true);
			cloth->SetSimulate(false);
			cloth->SetSelfCollide(false);
			sprites[iHANDLE].SetVisibility(255);
			sprites[iCOLLIDE].SetVisibility(255/2);
			sprites[iGRAVITY].SetVisibility(255/2);
			HandleMode = true;
		}
		else
		{
			cloth->DeselectRow();
			cloth->SetManipulate(false);
			sprites[iHANDLE].SetVisibility(255/2);
			HandleMode = false;
		}
		break;
	}
	case iCOLLIDE:
	{
		if(!HandleMode)
		{
			if(cloth->IsSelfColliding())
			{
				sprites[iCOLLIDE].SetVisibility(255/2);
				cloth->SetSelfCollide(false);
			}
			else
			{
				sprites[iCOLLIDE].SetVisibility(255);
				cloth->SetSelfCollide(true);
			}
		}
		break;
	}
	case iKEYS:
	{
		sprites[iKEYLIST].ToggleVisibility(0);
		sprites[iKEYS].ToggleVisibility();
		break;
	}
	case iKEYLIST:
	{
		sprites[iKEYLIST].ToggleVisibility(0);
		sprites[iKEYS].ToggleVisibility();
		break;
	}
	case iMOVE:
	{
		if(MoveBall)
			MoveBall = false;
		else
			MoveBall = true;
		sprites[iMOVE].ToggleVisibility();
		break;
	}
	}	
}
//=============================================================================
//PRIVATE EVENTS
//=============================================================================
void GAME::CreateNewCloth()
{
	//cloth->ReleaseCloth();
	delete cloth;
	cloth = new CLOTH();
	cloth->OnLoad(clothDimensions,clothDimensions,clothSize,&shaders[xCLOTH]);
	cloth->AddTexture(".\\Resources\\Models\\square.png");

	HandleMode = false;
	sprites[iSHOWVERTS].SetVisibility(255);
	sprites[iGRAVITY].SetVisibility(255/2);
	sprites[iHANDLE].SetVisibility(255/2);
	sprites[iCOLLIDE].SetVisibility(255/2);
}
void GAME::UpdateText()
{
	s.seekp(0);
	s.str("");
	text[wSTATS].SetText(timestep + damp + vertcount + size + iterations);
}
//=============================================================================
//Check whether meshes have been clicked
//=============================================================================
void GAME::OnClickMesh()
{
	cloth->MousePickingTest();
}