#pragma once
#include "Engine/Core/EngineCommonH.hpp"

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

    string ToString();

private:
    class ProfilerReportEntry* m_rootEntry;

    bool m_isFlat = false;
    double m_frameTime = 0;
};
