#include "Engine/Time/Clock.hpp"
#include "Engine/Core/ContainerUtils.hpp"

Clock::Clock( Clock* parent )
{
    SetParent( parent );
}

Clock::~Clock()
{
    if( m_parentClock )
        m_parentClock->RemoveChild( this );

    for( auto& child : m_childrenClocks )
        child->SetParent( nullptr );
}

void Clock::Update( double ds )
{
    if( m_paused )
        ds = 0;
    else
        ++m_frameCount;
    m_deltaSeconds = ds * m_timeScale;
    m_deltaSecondsF = (float) m_deltaSeconds;
    m_timeSinceStartup += m_deltaSeconds;
    m_timeSinceStartupF += m_deltaSecondsF;

    for( auto& child : m_childrenClocks )
        child->Update( m_deltaSeconds );
}

void Clock::SetParent( Clock* parent )
{
    m_parentClock = parent;
    if( parent )
        parent->AddChild( this );
}

void Clock::AddChild( Clock* child )
{
    m_childrenClocks.push_back( child );
}

void Clock::RemoveChild( Clock* child )
{
    ContainerUtils::EraseOneValue( m_childrenClocks, child );
}
