#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <map>

namespace Profiler
{
struct Measurement;
}

class ProfilerReportEntry
{
private:
    static constexpr int CHILD_STR_INDENT = 1;
public:
    ProfilerReportEntry( const String& name );
    ~ProfilerReportEntry();

    void PopulateTreeR( Profiler::Measurement *node );

    void AccumulateData( Profiler::Measurement *node );

    void FinalizeDataR( double frameTime );

    void PopulateFlatR( Profiler::Measurement *node, ProfilerReportEntry *rootEntry );

    ProfilerReportEntry* GetOrCreateChild( const String& id );

    String GernerateStringR( int indent = 0 );

    size_t GetTotalStringSizeR();

    void AppendToStringR( String& appendTo );

public:
    String m_name;
    uint m_callCount;
    double m_totalTime; // inclusive time;
    double m_selfTime;  // exclusive time
    double m_totalPercentTime;
    double m_selfPercentTime;

    String m_reportString;

    // if you care about new piece data - add time;
    // mean
    // median
    // high water
    // low water

    ProfilerReportEntry *m_parent;
    std::map<String, ProfilerReportEntry*> m_children;

};



