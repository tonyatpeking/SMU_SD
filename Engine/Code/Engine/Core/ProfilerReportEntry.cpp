#include "Engine/Core/ProfilerReportEntry.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

ProfilerReportEntry::ProfilerReportEntry( const String& name )
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

void ProfilerReportEntry::FinalizeDataR( double frameTime )
{
    double childrenTime = 0;
    for( auto& entryPair : m_children )
    {
        ProfilerReportEntry* child = entryPair.second;
        childrenTime += child->m_totalTime;
        child->FinalizeDataR( frameTime );
    }

    m_totalPercentTime = m_totalTime / frameTime * 100;

    m_selfTime = m_totalTime - childrenTime;
    // this only happens for flat view root node
    if( m_selfTime < 0 )
        m_selfTime = 0;
    m_selfPercentTime = m_selfTime / frameTime * 100;
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
        Profiler::Measurement* currentChild = firstChild;
        do
        {
            ProfilerReportEntry *entry = rootEntry->
                GetOrCreateChild( currentChild->m_name );

            entry->PopulateFlatR( currentChild, rootEntry );
            currentChild = currentChild->m_next;
        } while( currentChild != firstChild );
    }
}

ProfilerReportEntry* ProfilerReportEntry::GetOrCreateChild( const String& id )
{
    ProfilerReportEntry* entry = nullptr;
    if( !ContainerUtils::Contains( m_children, id ) )
    {
        entry = new ProfilerReportEntry( id );
        m_children[id] = entry;
        entry->m_parent = this;
        entry->m_name = id;
    }

    return m_children[id];
}

String ProfilerReportEntry::GernerateStringR( int indent /*= 0 */ )
{
    m_reportString = Stringf(
        "%*s%-*s %-7i %-5.1f%% [%-7.2f ms] %-5.1f%% [%-7.2f ms] \n",
                    indent, "",
                    64 - indent, m_name.c_str(),
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

void ProfilerReportEntry::AppendToStringR( String& appendTo )
{
    appendTo.append( m_reportString );
    for( auto& pair : m_children )
    {
        pair.second->AppendToStringR( appendTo );
    }
}
