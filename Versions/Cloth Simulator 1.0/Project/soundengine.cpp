
#include "soundengine.h"
#include "global.h"

//=============================================================
//Constructor
//=============================================================
void SOUNDENGINE::OnLoad()
{
	(DirectSoundCreate8(NULL, &soundDevice, NULL)); // create sound device
	(soundDevice->SetCooperativeLevel(gwd.hWnd, DSSCL_NORMAL)); // set coop level

	currentTrack = NULL;
	overTopTrack = NULL;

	//SOUND EFFECTS
	for (int i = 0; i < MAX_SOUNDEFFECTS; i++)
	{
		IDirectSoundBuffer* temp = NULL;
		soundEffects.push_back(temp);
	}

	InitialiseSoundBuffer(&soundEffects[oBUTTON], ".\\Resources\\Sounds\\Click.wav");

	//MUSIC TRACKS
	for (int i = 0; i < MAX_MUSIC; i++)
	{
		IDirectSoundBuffer* temp = NULL;
		musicTracks.push_back(temp);
	}

	InitialiseSoundBuffer(&musicTracks[sMAIN], ".\\Resources\\Music\\MENU_Babylon_loop.wav");
}

//=============================================================
//destructor
//=============================================================
SOUNDENGINE::~SOUNDENGINE(void)
{
	//SOUND EFFECTS
	for (int i = 0; i < MAX_SOUNDEFFECTS; i++)
		ReleaseCOM(soundEffects[i])

	//MUSIC
	for (int i = 0; i < MAX_MUSIC; i++)
		ReleaseCOM(musicTracks[i])

	ReleaseCOM(soundDevice);
}

//=============================================================
//play a single sound effect once
//=============================================================
void SOUNDENGINE::PlaySoundEffect(IDirectSoundBuffer * se, int vol)
{
	se->SetVolume(vol);
	se->Play(0,0,0);
}

//=============================================================
//Play a music track over the top of another track
//=============================================================
void SOUNDENGINE::PlayTrackOverTop(IDirectSoundBuffer * bgm, int vol)
{
	if ((overTopTrack != bgm) || (overTopTrack == NULL)) //if not the same track
	{
		overTopTrack = bgm;
		overTopTrack->SetVolume(vol);
		overTopTrack->Play(0,0,0);
	}
	else if(!this->IsOverTopPlaying())
	{
		overTopTrack->SetVolume(vol);
		overTopTrack->Play(0,0,0);
	}
	else
		overTopTrack->SetVolume(vol);
}


//=============================================================
//check if any overtop music is playing
//=============================================================
bool SOUNDENGINE::IsOverTopPlaying()
{
	if (overTopTrack == NULL)
		return false;

	DWORD bufferStatus;
	overTopTrack->GetStatus(&bufferStatus);
	if(bufferStatus & DSBSTATUS_PLAYING)
		return true;
	else
		return false;
}

//=============================================================
//Stop track over the top of another track
//=============================================================
void SOUNDENGINE::StopTrackOverTop()
{
	if(overTopTrack != NULL)
	{
		static long vol;
		overTopTrack->GetVolume(&vol);
		if(vol > MIN_VOLUME)
		{
			vol-= FADE_SPEED;
			overTopTrack->SetVolume(vol);
		}
		else
		{
			overTopTrack->Stop();
			overTopTrack = NULL;
		}
	}
}

//=============================================================
//play a single music track once
//=============================================================
void SOUNDENGINE::PlayTrack(IDirectSoundBuffer * bgm, int vol)
{
	if (currentTrack != bgm) //if not the same track
	{
		Vol = vol;
		if (currentTrack != NULL)
			currentTrack->Stop();
		currentTrack = bgm;
		currentTrack->SetVolume(Vol);
		currentTrack->Play(0,0,0);
	}
	else
		RepeatCurrentTrack(Vol);
}

//=============================================================
//play a random track from track list
//=============================================================
void SOUNDENGINE::PlayRandomTrack(int vol)
{
	static int cr = -1;

	int r = rand() % MAX_MUSIC; //for 0 to MAX_RANGE
	while (cr == r); //check if track is same as last played
		r = rand() % MAX_MUSIC; //for 0 to MAX_RANGE
	
	cr = r;
	PlayTrack(musicTracks[r],vol);

}

//=============================================================
//check if a specific track is playing
//=============================================================
bool SOUNDENGINE::IsTrackPlaying(IDirectSoundBuffer * bgm)
{
	if (currentTrack == NULL)
		return false;

	DWORD bufferStatus;
	bgm->GetStatus(&bufferStatus);
	if(bufferStatus & DSBSTATUS_PLAYING)
		return true;
	else
		return false;
}

//=============================================================
//check if any music is playing
//=============================================================
bool SOUNDENGINE::IsMusicPlaying()
{
	if (currentTrack == NULL)
		return false;

	DWORD bufferStatus;
	currentTrack->GetStatus(&bufferStatus);
	if(bufferStatus & DSBSTATUS_PLAYING)
		return true;
	else
		return false;
}

//=============================================================
//repeat the last track played
//=============================================================
void SOUNDENGINE::RepeatCurrentTrack(int vol)
{
	currentTrack->SetVolume(vol);
	currentTrack->Play(0,0,0);
};


//=================================================================================================
//INITIALISE SOUNDBUFFER (DON'T EDIT)
//=================================================================================================

void SOUNDENGINE::InitialiseSoundBuffer(IDirectSoundBuffer **secondaryBuffer, LPSTR filename)
{
	// open WAV file, make sure its a WAV file, etc
	HMMIO wavefile;
	if((wavefile = mmioOpen(&filename[0], NULL, MMIO_ALLOCBUF | MMIO_READ)) == NULL)
		( ! DS_OK );

	// find a "RIFF" chunk that contains the "WAVE" type
	MMCKINFO parentChunkInfo;
    parentChunkInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');

    if(mmioDescend(wavefile, (LPMMCKINFO) &parentChunkInfo, NULL, MMIO_FINDRIFF))
    {
        // error: file is not a WAV file
        mmioClose(wavefile, 0);
		( ! DS_OK );
    }
	
	// find a "FMT" chunk, it must be a subchunk of the "RIFF" chunk. 
	MMCKINFO subChunkInfo;
    subChunkInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');

    if(mmioDescend(wavefile, &subChunkInfo, &parentChunkInfo, MMIO_FINDCHUNK))
    {
        // error: "FMT" chunk missing
        mmioClose(wavefile, 0);
		( ! DS_OK );
    }

    // Read the "FMT" chunk.
	PCMWAVEFORMAT pcmWaveFMT;
    ZeroMemory(&pcmWaveFMT, sizeof(PCMWAVEFORMAT)); 

    if(mmioRead(wavefile, (HPSTR) &pcmWaveFMT, subChunkInfo.cksize) != subChunkInfo.cksize)
	{
        // error: failed to read "FMT" chunk
        mmioClose(wavefile, 0);
		( ! DS_OK );
    }

	if(pcmWaveFMT.wf.wFormatTag != WAVE_FORMAT_PCM)
	{
		// error: not PCM format, just use PCM for now
        mmioClose(wavefile, 0);
		( ! DS_OK );
	}


	// get WAVex format (i.e., description),
	// we just read most of it and placed it in pcmWaveFMT
	WAVEFORMATEX waveFormat;
    ZeroMemory(&waveFormat, sizeof(WAVEFORMATEX));
	memcpy(&waveFormat, &pcmWaveFMT, sizeof(pcmWaveFMT));
	waveFormat.cbSize = 0;

	// Ascend out of the "FMT" subchunk. 
    mmioAscend(wavefile, &subChunkInfo, 0); 
 
    // Find the data subchunk
    subChunkInfo.ckid = mmioFOURCC('d', 'a', 't', 'a'); 
    if(mmioDescend(wavefile, &subChunkInfo, &parentChunkInfo, MMIO_FINDCHUNK))
    {
        // error: WAV file has no 'data' chunk
        mmioClose(wavefile, 0); 
		( ! DS_OK );
    }

	// check the size of 'data'
    if(subChunkInfo.cksize == 0)
	{
		// error: data missing
        mmioClose(wavefile, 0);
		( ! DS_OK );
    }

	// secondary buffer description
	DWORD dataSize = subChunkInfo.cksize;                     // temp

	DSBUFFERDESC secondaryBufferDesc;
    ZeroMemory(&secondaryBufferDesc, sizeof(DSBUFFERDESC));
    secondaryBufferDesc.dwSize = sizeof(DSBUFFERDESC);
    secondaryBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;
    secondaryBufferDesc.dwBufferBytes = dataSize;             // data size
	secondaryBufferDesc.guid3DAlgorithm = DS3DALG_DEFAULT;
    secondaryBufferDesc.lpwfxFormat = &waveFormat;            // the WAV description

	// create the secondary buffer
	(soundDevice->CreateSoundBuffer(&secondaryBufferDesc, secondaryBuffer, NULL));

	// lock secondary buffer (so we can read the file and write to the sound buffer)
	VOID *lockedBuffer;
	DWORD lockedBufferSize;
	((*secondaryBuffer)->Lock(0, dataSize,
							&lockedBuffer, &lockedBufferSize,
							NULL, NULL, DSBLOCK_ENTIREBUFFER));

	// read WAV data and place it into the secondary buffer
	if(mmioRead(wavefile, (HPSTR)lockedBuffer, dataSize) != dataSize)
	{
		//error: reading data failed
		mmioClose(wavefile, 0);
		( ! DS_OK );
	}

	// close WAV file
    mmioClose(wavefile, 0);

	// unlock
	((*secondaryBuffer)->Unlock(lockedBuffer, lockedBufferSize, NULL, NULL));
}