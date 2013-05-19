/********************************************************************
 File: anisprite.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: Animated sprite class
 *******************************************************************/

#ifndef ANISPRITE_H_
#define ANISPRITE_H_

#include "sprite.h"
#include "includes.h"
#define SPEED_FACTOR 1

class ANISPRITE : public SPRITE
{
protected:
	int cols, rows, frame;
	float speedCounter;
	bool playing;

public:
	virtual void ANISPRITE::OnLoad(char * filename, int Vis, int Xwidth, int Yheight, float X, float Y, int height, int width, int Columns, int Rows);
	virtual void OnDraw(float scale = NULL);

	void SetFrame(int f){frame = f;};
	void SetPlay(bool b){playing = b;};
	bool IsPlaying(){return playing;};
	bool PlayAnimation(float speed);
	void Left(float n){x-=n;};
	void Right(float n){x+=n;};
	void Up(float n){y-=n;};
	void Down(float n){y+=n;};
};

#endif
