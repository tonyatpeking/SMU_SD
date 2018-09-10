#pragma once
#include "Game/EngineBuildPreferences.hpp"

// Macros
#if defined( PROFILING_ENABLED )

#define PROFILE_LOG_SCOPE(tag) ProfileLogScoped __timer_##__LINE__ ## (#tag)
#define PROFILE_LOG_SCOPE_FUNCTION() ProfileLogScoped __timer_##__LINE__ ## (__FUNCTION__)

#else

#define PROFILE_LOG_SCOPE(tag) { (void)( #tag ); }
#define PROFILE_LOG_SCOPE_FUNCTION() { ; }

#endif

class ProfileLogScoped
{
public:

    ProfileLogScoped( const char* tag );
    ~ProfileLogScoped();

private:

    double m_timeStart = 0.0;
    // don't worry about storage - this is stack based,
    // so any pointer we pass in will still be in scope;
    const char* m_tag;
};