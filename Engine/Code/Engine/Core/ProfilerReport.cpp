#include "Engine/Core/ProfilerReport.hpp"
#include "Engine/Core/ProfilerReportEntry.hpp"
#include "Engine/Core/Profiler.hpp"


void ProfilerReport::GenerateReportTreeFromFrame( Profiler::Measurement *rootMeasure )
{
    m_rootEntry = new ProfilerReportEntry( rootMeasure->m_id );
    m_rootEntry->PopulateTreeR( rootMeasure );
    m_rootEntry->FinalizeDataR( m_rootEntry->m_totalTime );
}

void ProfilerReport::GenerateReportFlatFromFrame( Profiler::Measurement *rootMeasure )
{
    m_rootEntry = new ProfilerReportEntry( rootMeasure->m_id );
    m_rootEntry->PopulateFlatR( rootMeasure, m_rootEntry );
    m_rootEntry->FinalizeDataR( m_rootEntry->m_totalTime );
}

void ProfilerReport::SortBySelfTime()
{

}

void ProfilerReport::SortByTotalTime()
{

}

double ProfilerReport::GetTotalFrameTime()
{
    return m_rootEntry->m_totalTime;
}
