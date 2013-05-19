// * ********************************************************************** * //
// * * GFXAppTimer.h                                                      * * //
// * * Author: Tim Wilkin                                                 * * //
// * * Created: 05/07/08     Last Modified: 12/07/09                      * * //
// * ********************************************************************** * //

#ifndef TIMER_H
#define TIMER_H

#include "includes.h"
#define DEFAULT_BUF_SIZE 20

class TIMER
{

private:

	INT64	m_ticksPerSecond;
	INT64	m_currentTime;
	INT64	m_lastTime;
	INT64	m_lastFPSUpdate;
	INT64	m_FPSUpdateInterval;
	UINT	m_numFrames;
	float	m_runningTime;
	float	m_timeElapsed;
	float	m_fps;
	BOOL	m_timerStopped;

	char		m_pFPSbuffer[DEFAULT_BUF_SIZE];
	RECT		textbox; 
    LPD3DXFONT	font;


public:
	
	TIMER();
	~TIMER();

	void	Start();
	void	Stop();
	void	Update();
	void	Reset();
	void	OnLostDevice();
	void	OnResetDevice();
	BOOL	IsRunning() { return !m_timerStopped; };
	float	GetFPS() { return m_fps; };
	float	GetRunningTime() { return m_runningTime; };
	float	GetElapsedTime() { return m_timerStopped ? 0.0f : m_timeElapsed; };
	void	CreateFont();
	void	PrintTimer();

};

#endif