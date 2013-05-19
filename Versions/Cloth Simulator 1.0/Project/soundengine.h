/********************************************************************
 File: soundengine.h
 Author: Kara Jensen                   
 Date Created: 5/3/10               
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments:	Code referenced from Frank Luna/Owen Knight
 ********************************************************************/

#ifndef _SOUNDENGINE_H
#define _SOUNDENGINE_H

#define MAX_VOLUME -1000
#define MIN_VOLUME -10000
#define FADE_SPEED 100

#include "includes.h"
#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }

class SOUNDENGINE
{
private:
	IDirectSound8 *soundDevice;
	DWORD startPosition;
	IDirectSoundBuffer *currentTrack;
	IDirectSoundBuffer *overTopTrack;
	long Vol;

public:

	vector<IDirectSoundBuffer*> soundEffects;
	vector<IDirectSoundBuffer*> musicTracks;

	//METHODS
	~SOUNDENGINE();
	void OnLoad();
	void InitialiseSoundBuffer(IDirectSoundBuffer **secondaryBuffer, LPSTR filename);
	void PlaySoundEffect(IDirectSoundBuffer * se, int vol);
	void PlayTrack(IDirectSoundBuffer * bgm, int vol);
	void PlayRandomTrack(int vol);
	void RepeatCurrentTrack(int vol);
	bool IsTrackPlaying(IDirectSoundBuffer * bgm);
	bool IsMusicPlaying();
	void StopTrackOverTop();
	void PlayTrackOverTop(IDirectSoundBuffer * bgm, int vol);
	bool IsOverTopPlaying();
};
#endif