#pragma once
#include "Engine/Core/EngineCommonH.hpp"


class Clock
{
public:
    static Clock* GetRealTimeClock();
    static void SetRealTimeClock( Clock* clock );
    Clock() {};
    Clock( Clock* parent );
    ~Clock();
    void Update( double ds );
    void SetTime( double seconds );
    double GetTimeSinceStartup() { return m_timeSinceStartup; };
    float GetTimeSinceStartupF() { return m_timeSinceStartupF; };
    double GetDeltaSeconds() { return m_deltaSeconds; };
    float GetDeltaSecondsF() { return m_deltaSecondsF; };
    void SetTimeScale( double timeScale ) { m_timeScale = timeScale; };
    double GetTimeScale() { return m_timeScale; };
    void SetParent( Clock* parent );
    void SetPause( bool pause ) { m_paused = pause; };
    bool IsPaused() { return m_paused; };
private:
    void AddChild( Clock* child );
    void RemoveChild( Clock* child );
    double m_timeScale = 1.;
    double m_timeSinceStartup = 0.;
    float m_timeSinceStartupF = 0.f;
    double m_deltaSeconds = 0.;
    float m_deltaSecondsF = 0.f;
    bool m_paused = false;
    int m_frameCount = 0;
    Clock* m_parentClock = nullptr;
    vector<Clock*> m_childrenClocks;

};
