#include "Engine/Core/ProfilerReportEntry.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/ContainerUtils.hpp"

ProfilerReportEntry::ProfilerReportEntry( const String& id )
    : m_id( id )
{

}

void ProfilerReportEntry::PopulateTreeR( Profiler::Measurement *node )
{
    AccumulateData( node );

    Profiler::Measurement* firstChild = node->m_firstChild;
    if( nullptr == firstChild )
    {
        return;
    }
    else
    {
        Profiler::Measurement* currentChild = firstChild;
        do
        {
            ProfilerReportEntry *entry = GetOrCreateChild( currentChild->m_id );
            entry->PopulateTreeR( currentChild );
            currentChild = currentChild->m_next;
        } while( currentChild != firstChild );
    }
}

void ProfilerReportEntry::AccumulateData( Profiler::Measurement *node )
{
    m_callCount++;
    m_totalTime += node->GetElapsedTime();
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
    m_selfPercentTime = m_selfTime / frameTime * 100;
}

void ProfilerReportEntry::PopulateFlatR( Profiler::Measurement *node,
                                        ProfilerReportEntry *rootEntry )
{
    AccumulateData( node );

    Profiler::Measurement* firstChild = node->m_firstChild;
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
                GetOrCreateChild( currentChild->m_id );

            PopulateFlatR( currentChild, rootEntry );
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
    }

    return m_children[id];
}
