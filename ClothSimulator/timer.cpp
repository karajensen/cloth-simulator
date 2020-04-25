////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - timer.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "timer.h"

namespace
{
    const double DT_INCREASE = 0.001; ///< Amount to change the forced deltatime
    const double DT_MAXIMUM = 0.03;   ///< Maximum allowed deltatime
    const double DT_MINIMUM = 0.01;   ///< Minimum allowed deltatime
}

Timer::Timer(EnginePtr engine)
    : m_frequency(0.0)
    , m_previousTime(0.0)
    , m_deltaTime(0.0)
    , m_deltaTimeCounter(0.0)
    , m_fps(0)
    , m_fpsCounter(0)
    , m_engine(engine)
    , m_forceDeltatime(false)
    , m_forcedDeltatime(0.04)
{
}

void Timer::StartTimer()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&m_timer);

    m_frequency = static_cast<double>(frequency.QuadPart);
    m_previousTime = static_cast<double>(m_timer.QuadPart);
}

void Timer::UpdateTimer()
{
    QueryPerformanceCounter(&m_timer);
    double currentTime = static_cast<double>(m_timer.QuadPart);

    double deltatime = (currentTime - m_previousTime) / m_frequency;
    m_deltaTimeCounter += deltatime;
    if (m_deltaTimeCounter >= 1.0) //one second has passed
    {
        m_deltaTimeCounter = 0.0;
        m_fps = m_fpsCounter;
        m_fpsCounter = 0;
    }

    m_deltaTime = max(deltatime, DT_MINIMUM);
    m_deltaTime = min(m_deltaTime, DT_MAXIMUM);

    if(m_engine->diagnostic()->AllowDiagnostics(Diagnostic::TEXT))
    {
        m_engine->diagnostic()->UpdateText(Diagnostic::TEXT,
            "FramePerSec", Diagnostic::WHITE, StringCast(m_fps));

        m_engine->diagnostic()->UpdateText(Diagnostic::TEXT,
            "DeltaTime", Diagnostic::WHITE, StringCast(deltatime));

        m_engine->diagnostic()->UpdateText(Diagnostic::TEXT,
            "CappedDeltaTime", Diagnostic::WHITE, StringCast(m_deltaTime));

        if(m_forceDeltatime)
        {
            m_engine->diagnostic()->UpdateText(Diagnostic::TEXT,
                "ForcedDeltaTime", Diagnostic::YELLOW, 
                StringCast(m_forcedDeltatime), true);
        }
    }
    
    ++m_fpsCounter; //increment frame counter
    m_previousTime = currentTime;
}

float Timer::GetDeltaTime() const 
{ 
    return static_cast<float>(m_forceDeltatime ? m_forcedDeltatime : m_deltaTime); 
}

void Timer::ToggleForceDeltatime()
{
    m_forceDeltatime = !m_forceDeltatime;
}

void Timer::ChangeDeltatime(bool increase)
{
    m_forcedDeltatime += increase ? DT_INCREASE : -DT_INCREASE;
}