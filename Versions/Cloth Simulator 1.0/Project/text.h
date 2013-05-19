/********************************************************************
 File: text.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: Text class
 *******************************************************************/

#ifndef DTEXT_H_
#define DTEXT_H_


#include "includes.h"
struct COLOUR { int red, green, blue, alpha; };
enum {alignLEFT = 0, alignRIGHT, alignCENTER};

class DTEXT
{
protected:
	int xLeft,yTop,xRight,yBottom;
	RECT textbox; 
    LPD3DXFONT font;
	string text;
	float visCounter;
	COLOUR colour;
	UINT align;

public:
	DTEXT(){ font = NULL; };
	~DTEXT();
	void OnDraw();
	void OnLoad(char * family, bool italic, int weight, int size, int xtl, int ytl, int xbr, int ybr);
	
	void SetText(string x){text = x;};
	void SetText(char * x){text = x;};
	void SetColour(int a, int r, int g, int b);
	void SetAlign(int alignment);
	void SetVisibility(int v);
	void FadeVisibility(int speed, int v);
	int GetVisibility(){return colour.alpha;};
	LPD3DXFONT & GetFont(){return font;};
};

#endif