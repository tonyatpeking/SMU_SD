#include "Engine/Profiler/ProfilerReport.hpp"
#include "Engine/Profiler/ProfilerReportEntry.hpp"
#include "Engine/Profiler/Profiler.hpp"


ProfilerReport::~ProfilerReport()
{
    delete m_rootEntry;
}

void ProfilerReport::GenerateReportTreeFromFrame( Profiler::Measurement *frame )
{
    m_rootEntry = new ProfilerReportEntry( frame->m_name );
    m_rootEntry->PopulateTreeR( frame );
    m_rootEntry->FinalizeTimesR();
    m_frameTime = m_rootEntry->m_totalTime;
    m_rootEntry->FinalizePercentagesR( m_frameTime );
}

void ProfilerReport::GenerateReportFlatFromFrame( Profiler::Measurement *frame )
{
    ProfilerReportEntry frameEntry = ProfilerReportEntry( frame->m_name );
    frameEntry.PopulateTreeR( frame );
    frameEntry.FinalizeTimesR();
    m_rootEntry = new ProfilerReportEntry( "root" );
    frameEntry.CollapesTreeToFlatR( m_rootEntry );
    m_frameTime = frameEntry.m_totalTime;
    m_rootEntry->FinalizePercentagesR( m_frameTime );
}

void ProfilerReport::SortBySelfTime()
{
    m_rootEntry->SortChildernSelfTimeR();
}

void ProfilerReport::SortByTotalTime()
{
    m_rootEntry->SortChildernTotalTimeR();
}

double ProfilerReport::GetTotalFrameTime()
{
    return m_frameTime;
}

string ProfilerReport::ToString()
{
    if( m_rootEntry->m_reportString.empty() )
        m_rootEntry->GernerateStringR();

    string str;
    size_t stringTotalLength = m_rootEntry->GetTotalStringSizeR();
    str.reserve( stringTotalLength );
    m_rootEntry->AppendToStringR( str );
    return str;
}
