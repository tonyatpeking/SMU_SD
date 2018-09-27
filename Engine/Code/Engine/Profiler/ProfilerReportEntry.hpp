#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include <map>
#include <functional>

namespace Profiler
{
struct Measurement;
}

class ProfilerReportEntry
{
private:
    static constexpr int CHILD_STR_INDENT = 1;
public:
    ProfilerReportEntry( const string& name );
    ~ProfilerReportEntry();

    void PopulateTreeR( Profiler::Measurement *node );

    void AccumulateData( Profiler::Measurement *node );

    void FinalizeTimesR();

    // Call after FinalizeTimesR for tree and after CollapesTreeToFlatR for flat
    void FinalizePercentagesR( double frameTime );

    // root Entry should be a clean entry without children
    // call after FinalizeTimesR
    void CollapesTreeToFlatR( ProfilerReportEntry *rootEntry );

    void PopulateFlatR( Profiler::Measurement *node, ProfilerReportEntry *rootEntry );

    ProfilerReportEntry* GetOrCreateChild( const string& id );

    string GernerateStringR( int indent = 0 );

    size_t GetTotalStringSizeR();

    // this is done according to sorting order
    void AppendToStringR( string& appendTo );

    void SortChildrenR(
        std::function<bool( ProfilerReportEntry*, ProfilerReportEntry* )> comparer );

    void SortChildernTotalTimeR();
    void SortChildernSelfTimeR();


public:
    string m_name;
    uint m_callCount;
    double m_totalTime = 0; // inclusive time;
    double m_selfTime = 0;  // exclusive time
    double m_totalPercentTime = 0;
    double m_selfPercentTime = 0;

    string m_reportString;

    // if you care about new piece data - add time;
    // mean
    // median
    // high water
    // low water

    ProfilerReportEntry *m_parent;
    map<string, ProfilerReportEntry*> m_children;
    vector <ProfilerReportEntry*> m_sortedChildren;
};



