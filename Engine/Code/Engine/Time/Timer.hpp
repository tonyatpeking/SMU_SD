#pragma once

class Clock;

class Timer
{
public:
	Timer(Clock* clock, float lapTime);
	~Timer(){};

    void SetLapTime( float lapTime ) { m_lapTime = lapTime; };
    void Reset();

    bool PeekOneLap();
    bool PopOneLap();
    int PopAllLaps();

    float m_timeOfStartLaps = 0;
    float m_lapTime = 1;
    Clock* m_clock = nullptr;


private:

};
