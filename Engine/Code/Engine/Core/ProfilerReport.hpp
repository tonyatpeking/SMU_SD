#pragma once
#include "Engine/Core/EngineCommon.hpp"

namespace Profiler
{
struct Measurement;
}

class ProfilerReport
{
public:
    ProfilerReport() {};
    ~ProfilerReport();

    void GenerateReportTreeFromFrame( Profiler::Measurement *rootMeasure );

    void GenerateReportFlatFromFrame( Profiler::Measurement *rootMeasure );

    void SortBySelfTime();

    void SortByTotalTime();

    double GetTotalFrameTime();

    String ToString();

public:
    class ProfilerReportEntry* m_rootEntry;
private:

};
