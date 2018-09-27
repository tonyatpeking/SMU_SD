#include "Engine/Profiler/ProfilerReportEntry.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/String/StringUtils.hpp"
#include <algorithm>

ProfilerReportEntry::ProfilerReportEntry( const string& name )
    : m_name( name )
{

}

ProfilerReportEntry::~ProfilerReportEntry()
{
    ContainerUtils::DeletePointers( m_children );
}

void ProfilerReportEntry::PopulateTreeR( Profiler::Measurement *measurement )
{
    AccumulateData( measurement );

    Profiler::Measurement* firstChild = measurement->m_firstChild;
    if( nullptr == firstChild )
    {
        return;
    }
    else
    {
        Profiler::Measurement* currentChild = firstChild;
        do
        {
            ProfilerReportEntry *entry = GetOrCreateChild( currentChild->m_name );
            entry->PopulateTreeR( currentChild );
            currentChild = currentChild->m_next;
        } while( currentChild != firstChild );
    }
}

void ProfilerReportEntry::AccumulateData( Profiler::Measurement *measurement )
{
    m_callCount++;
    m_totalTime += measurement->GetElapsedTime();
    // m_percent_time = ?; // figure it out later;
}

void ProfilerReportEntry::FinalizeTimesR()
{
    double childrenTime = 0;
    for( auto& entryPair : m_children )
    {
        ProfilerReportEntry* child = entryPair.second;
        childrenTime += child->m_totalTime;
        child->FinalizeTimesR();
    }

    m_selfTime = m_totalTime - childrenTime;
    // this only happens for flat view root node
    if( m_selfTime < 0 )
        m_selfTime = 0;
}

void ProfilerReportEntry::FinalizePercentagesR( double frameTime )
{
    m_totalPercentTime = m_totalTime / frameTime * 100;
    m_selfPercentTime = m_selfTime / frameTime * 100;
    for ( auto& entry : m_sortedChildren )
    {
        entry->FinalizePercentagesR(frameTime);
    }
}

void ProfilerReportEntry::CollapesTreeToFlatR( ProfilerReportEntry *rootEntry )
{
    ProfilerReportEntry* entry = rootEntry->GetOrCreateChild( m_name );
    entry->m_totalTime += m_totalTime;
    entry->m_selfTime += m_selfTime;
    entry->m_callCount += m_callCount;
    for ( auto& child : m_sortedChildren )
    {
        child->CollapesTreeToFlatR( rootEntry );
    }
}

void ProfilerReportEntry::PopulateFlatR( Profiler::Measurement *measurement,
                                         ProfilerReportEntry *rootEntry )
{
    AccumulateData( measurement );

    Profiler::Measurement* firstChild = measurement->m_firstChild;
    if( nullptr == firstChild )
    {
        return;
    }
    else
    {
        Profiler::Measurement* currentChildMeasure = firstChild;
        do
        {
            ProfilerReportEntry *entry = rootEntry->
                GetOrCreateChild( currentChildMeasure->m_name );

            entry->PopulateFlatR( currentChildMeasure, rootEntry );
            currentChildMeasure = currentChildMeasure->m_next;
        } while( currentChildMeasure != firstChild );
    }
}

ProfilerReportEntry* ProfilerReportEntry::GetOrCreateChild( const string& id )
{
    ProfilerReportEntry* entry = nullptr;
    if( !ContainerUtils::Contains( m_children, id ) )
    {
        entry = new ProfilerReportEntry( id );
        m_children[id] = entry;
        entry->m_parent = this;
        entry->m_name = id;
        m_sortedChildren.push_back( entry );
    }

    return m_children[id];
}

string ProfilerReportEntry::GernerateStringR( int indent /*= 0 */ )
{
    m_reportString = Stringf(
        "%*s%-*s %-7i %-5.1f%% [%-7.2f ms] %-5.1f%% [%-7.2f ms] \n",
                    indent, "",
                    62 - indent, m_name.c_str(),
                    m_callCount ,
                    m_totalPercentTime,
                    m_totalTime * 1000,
                    m_selfPercentTime,
                    m_selfTime * 1000 );

    for ( auto& pair : m_children )
    {
        ProfilerReportEntry* childEntry = pair.second;
        childEntry->GernerateStringR( indent + CHILD_STR_INDENT );
    }

    return m_reportString;
}

size_t ProfilerReportEntry::GetTotalStringSizeR()
{
    size_t childrenSize = 0;
    for ( auto& pair : m_children )
    {
        childrenSize += pair.second->GetTotalStringSizeR();
    }
    return m_reportString.size() + childrenSize;
}

void ProfilerReportEntry::AppendToStringR( string& appendTo )
{
    appendTo.append( m_reportString );
    for( auto& entry : m_sortedChildren )
    {
        entry->AppendToStringR( appendTo );
    }
}

// compare functions
namespace
{
bool CompareTotalTime( ProfilerReportEntry* lhs, ProfilerReportEntry* rhs )
{
    return lhs->m_totalTime > rhs->m_totalTime;
}

bool CompareSelfTime( ProfilerReportEntry* lhs, ProfilerReportEntry* rhs )
{
    return lhs->m_selfTime > rhs->m_selfTime;
}

}

void ProfilerReportEntry::SortChildrenR( std::function<bool( ProfilerReportEntry*, ProfilerReportEntry* )> comparer )
{
    if( 0 == m_sortedChildren.size()  )
        return;
    std::sort( m_sortedChildren.begin(), m_sortedChildren.end(), comparer );

    for ( auto& entry : m_sortedChildren )
    {
        entry->SortChildrenR( comparer );
    }
}

void ProfilerReportEntry::SortChildernTotalTimeR()
{
    SortChildrenR( CompareTotalTime );
}

void ProfilerReportEntry::SortChildernSelfTimeR()
{
    SortChildrenR( CompareSelfTime );
}
