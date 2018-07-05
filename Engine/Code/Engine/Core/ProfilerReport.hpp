#pragma once

namespace Profiler
{
struct Measurement;
}

class ProfilerReport
{
public:
    ProfilerReport() {};
    ~ProfilerReport() {};

    void GenerateReportTreeFromFrame( Profiler::Measurement *rootMeasure );

    void GenerateReportFlatFromFrame( Profiler::Measurement *rootMeasure );

    void SortBySelfTime();

    void SortByTotalTime();

    double GetTotalFrameTime();


public:
    class ProfilerReportEntry* m_rootEntry;
private:

};
