
#include "text.h"
#include "global.h" //for d3ddev


//=============================================================
//Destructor
//=============================================================
DTEXT::~DTEXT()
{ 
	if (font != NULL)
		font->Release(); 
}

//=============================================================
//load a font
//=============================================================
void DTEXT::OnLoad(char * family, bool italic, int weight, int size, int xtl, int ytl, int xbr, int ybr)
{
	  if(FAILED(D3DXCreateFont(d3ddev,
							   size,
							   0, 
							   weight, //0-1000 weight
							   1, 
						       italic,  
							   DEFAULT_CHARSET, 
							   OUT_DEFAULT_PRECIS,  
							   ANTIALIASED_QUALITY,
							   DEFAULT_PITCH | FF_DONTCARE,
							   LPSTR(family),
							   &font)))
	  {
		MessageBox(NULL, TEXT("Text failed to load"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	  };

		text = ""; //initial text
		SetColour(255,255,255,255); //initial colour
		align = DT_LEFT; //initial alignment

		yBottom = ybr;
		xRight = xbr;
		xLeft = xtl;
		yTop = ytl;

		SetRect(&textbox,xLeft,yTop,xRight,yBottom);
	  
}

//=============================================================
//Draw text
//=============================================================
void DTEXT::OnDraw()
{	
	font->DrawTextA(NULL,
					LPSTR(ConvertSTR(text)),
					-1, //number of characters
					&textbox,
					DT_WORDBREAK|DT_NOCLIP|align,
					D3DCOLOR_ARGB(colour.alpha,
								  colour.red,
								  colour.green,
								  colour.blue));
}

//=============================================================
//set colour of text
//=============================================================
void DTEXT::SetColour(int a, int r, int g, int b)
{
	colour.alpha = a;
	colour.red = r;
	colour.blue = b;
	colour.green = g;
}

//=============================================================
//set alignment
//=============================================================
void DTEXT::SetAlign(int alignment)
{
	switch (alignment)
	{
		case alignCENTER:
		{
			align = DT_CENTER;
			break;
		}
		case alignRIGHT:
		{
			align = DT_RIGHT;
			break;
		}
		case alignLEFT:
		default:
		{
			align = DT_LEFT;
			break;
		}
	}
}

//=============================================================
//set transparency level
//=============================================================
void DTEXT::SetVisibility(int v)
{
	if (v < 0)
		v = 0;
	else if (v > 255)
		v = 255;

	colour.alpha = v;
}

//=============================================================
//fade the text
//=============================================================
void DTEXT::FadeVisibility(int speed, int v)
{
	if (colour.alpha != v)
	{
		if (v < colour.alpha)
		{
			int V = colour.alpha - speed;
			SetVisibility(V);
		}
		else
		{
			int V = colour.alpha + speed;
			SetVisibility(V);
		}
	}
}



