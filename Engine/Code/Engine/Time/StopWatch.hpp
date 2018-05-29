#pragma once

class Clock;

class StopWatch
{
public:
	StopWatch(){};
	~StopWatch(){};
    void Start();
    double Stop();
    double GetTime();
    void Reset();
private:

};
