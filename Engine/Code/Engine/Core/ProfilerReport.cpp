#include "Engine/Core/ProfilerReport.hpp"
#include "Engine/Core/ProfilerReportEntry.hpp"
#include "Engine/Core/Profiler.hpp"


ProfilerReport::~ProfilerReport()
{
    delete m_rootEntry;
}

void ProfilerReport::GenerateReportTreeFromFrame( Profiler::Measurement *rootMeasure )
{
    m_rootEntry = new ProfilerReportEntry( rootMeasure->m_name );
    m_rootEntry->PopulateTreeR( rootMeasure );
    m_rootEntry->FinalizeDataR( m_rootEntry->m_totalTime );
}

void ProfilerReport::GenerateReportFlatFromFrame( Profiler::Measurement *rootMeasure )
{
    m_rootEntry = new ProfilerReportEntry( rootMeasure->m_name );
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

String ProfilerReport::ToString()
{
    if( m_rootEntry->m_reportString.empty() )
        m_rootEntry->GernerateStringR();

    String str;
    size_t stringTotalLength = m_rootEntry->GetTotalStringSizeR();
    str.reserve( stringTotalLength );
    m_rootEntry->AppendToStringR( str );
    return str;
}
