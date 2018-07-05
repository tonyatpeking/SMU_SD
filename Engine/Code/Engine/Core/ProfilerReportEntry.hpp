#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <map>

namespace Profiler
{
struct Measurement;
}

class ProfilerReportEntry
{
public:
    ProfilerReportEntry( const String& id );

    void PopulateTreeR( Profiler::Measurement *node );

    void AccumulateData( Profiler::Measurement *node );

    void FinalizeDataR( double frameTime );

    void PopulateFlatR( Profiler::Measurement *node, ProfilerReportEntry *rootEntry );

    ProfilerReportEntry* GetOrCreateChild( const String& id );

public:
    String m_id;
    uint m_callCount;
    double m_totalTime; // inclusive time;
    double m_selfTime;  // exclusive time
    double m_totalPercentTime;
    double m_selfPercentTime;

    // if you care about new piece data - add time;
    // mean
    // median
    // high water
    // low water

    ProfilerReportEntry *m_parent;
    std::map<String, ProfilerReportEntry*> m_children;

};



