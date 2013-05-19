// * ********************************************************************** * //
// * * GFXAppTimer.cpp                                                    * * //
// * * Author: Tim Wilkin                                                 * * //
// * * Created: 05/07/08     Last Modified: 12/07/09                      * * //
// * ********************************************************************** * //

#include "timer.h"
#include "global.h" //for d3ddev

//=============================================================
//Constructor
//=============================================================
TIMER::TIMER()
{
	Reset();
    QueryPerformanceFrequency( (LARGE_INTEGER *)&m_ticksPerSecond );
    m_FPSUpdateInterval = m_ticksPerSecond >> 1;
}

//=============================================================
//Create font
//=============================================================
void TIMER::CreateFont()
{
	//intialise text
	 if(FAILED(D3DXCreateFont(d3ddev, 12, 0, 1000, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
							   DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &font)))
	 {
		MessageBox(NULL, TEXT("Text failed to load"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	 };

	 int topleftX = 0;
	 int topleftY = gwd.WINDOW_HEIGHT - 20;
	 int botrightX = gwd.WINDOW_WIDTH;
	 int botrightY = gwd.WINDOW_HEIGHT + gwd.WINDOW_WIDTH;
	 SetRect(&textbox, topleftX, topleftY, botrightX, botrightY); //set textbox for text

}

//=============================================================
//Destructor
//=============================================================
TIMER::~TIMER()
{
	if (font != NULL)
		font->Release(); 
}

//=============================================================
//Start the timer
//=============================================================
void TIMER::Start()
{
    if ( !m_timerStopped )
		return;
    QueryPerformanceCounter( (LARGE_INTEGER *)&m_lastTime );
    m_timerStopped = FALSE;
}

//=============================================================
//Stop the timer
//=============================================================
void TIMER::Stop()
{
    if ( m_timerStopped )
        return;
    INT64 stopTime = 0;
    QueryPerformanceCounter( (LARGE_INTEGER *)&stopTime );
    m_runningTime += (float)(stopTime - m_lastTime) / (float)m_ticksPerSecond;
    m_timerStopped = TRUE;
}

//=============================================================
//Update the timer
//=============================================================
void TIMER::Update()
{
	//Calculate the time elapsed since the last Update call)
	//Update the frames per second and total running time.     
	if ( m_timerStopped )
        return;

    // Get current count
    QueryPerformanceCounter( (LARGE_INTEGER *)&m_currentTime );
    
    m_timeElapsed = (float)(m_currentTime - m_lastTime) / (float)m_ticksPerSecond;
    m_runningTime += m_timeElapsed;
    m_lastTime = m_currentTime;

    // Update FPS over interval
    m_numFrames++;
    if ( m_currentTime - m_lastFPSUpdate >= m_FPSUpdateInterval )
    {
        float currentTime = (float)m_currentTime / (float)m_ticksPerSecond;
        float lastTime = (float)m_lastFPSUpdate / (float)m_ticksPerSecond;
        m_fps = (float)m_numFrames / (currentTime - lastTime);

        m_lastFPSUpdate = m_currentTime;
        m_numFrames = 0;
    }

}

//=============================================================
//Reset the timer
//=============================================================
void TIMER::Reset(){
    m_currentTime = m_lastTime = m_lastFPSUpdate = 0;
    m_numFrames = 0;
    m_runningTime = m_timeElapsed = m_fps = 0.0f;
    m_timerStopped = TRUE;
}

//=============================================================
//Print the timer
//=============================================================
void TIMER::PrintTimer()
{
	//copy to char buffer
	sprintf_s(m_pFPSbuffer,DEFAULT_BUF_SIZE,"FPS: %6.1f",this->GetFPS());
	
	font->DrawTextA(NULL,m_pFPSbuffer, -1, &textbox, DT_WORDBREAK|DT_NOCLIP|DT_CENTER,
					D3DCOLOR_ARGB(200,255,0,0));
}

//=============================================================
//On Reset Device
//=============================================================
void TIMER::OnResetDevice()
{
	int topleftX = 0;
	int topleftY = gwd.WINDOW_HEIGHT - 20;
	int botrightX = gwd.WINDOW_WIDTH;
	int botrightY = gwd.WINDOW_HEIGHT + gwd.WINDOW_WIDTH;
	SetRect(&textbox, topleftX, topleftY, botrightX, botrightY); //set textbox for text

	font->OnResetDevice();
}

//=============================================================
//On Lost Device
//=============================================================
void TIMER::OnLostDevice()
{
	font->OnLostDevice();
}