////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - timer.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <Windows.h>
#include "callbacks.h"

/**
* FPS class for determining frame rate and delta time
*/
class Timer
{
public:

    /**
    * Constructor
    */
    explicit Timer(EnginePtr engine);

    /**
    * Starts the initial ticking of the timer
    */
    void StartTimer();

    /**
    * Stops/Starts the timer to determine delta-time and fps
    */
    void UpdateTimer();

    /**
    * @return the time passed between frames (seconds)
    */
    float GetDeltaTime() const;

    /**
    * Toggles whether to use an explicitly set deltatime
    */
    void ToggleForceDeltatime();

    /**
    * Increases or decreases the explicitly set deltatime
    * @param increase Whether to increase deltatime or not
    */
    void ChangeDeltatime(bool increase);

private:

    EnginePtr m_engine;         ///< Callbacks for the rendering engine
    double m_frequency;         ///< The frequency of the high-resolution performance counter
    LARGE_INTEGER m_timer;      ///< The current time queried
    double m_previousTime;      ///< The previous time queried
    double m_deltaTime;         ///< Timestep in secs between frames
    double m_deltaTimeCounter;  ///< Combined timestep between frames up to 1 second
    unsigned int m_fps;         ///< Amount of frames rendered in 1 second
    unsigned int m_fpsCounter;  ///< Amount of frames rendered since delta time counter began
    bool m_forceDeltatime;      ///< Whether to force deltatime explicitly or not
    double m_forcedDeltatime;   ///< The value for the forced deltatime
};

