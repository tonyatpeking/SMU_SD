
#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Time/DateTime.hpp"
#include "Engine/String/StringUtils.hpp"

#define _SECOND ((__int64) 10000000)
#define _MINUTE (60 * _SECOND)
#define _HOUR   (60 * _MINUTE)
#define _DAY    (24 * _HOUR)

namespace
{

DateTime SysTimeToDateTime( const SYSTEMTIME& st )
{
    DateTime dt;
    dt.year = st.wYear;
    dt.month = st.wMonth;
    dt.dayOfWeek = st.wDayOfWeek;
    dt.day = st.wDay;
    dt.hour = st.wHour;
    dt.minute = st.wMinute;
    dt.second = st.wSecond;
    dt.milliseconds = st.wMilliseconds;

    return dt;
}

SYSTEMTIME DateTimeToSysTime( const DateTime& dt )
{
    SYSTEMTIME st;
    st.wYear = (WORD) dt.year;
    st.wMonth = (WORD) dt.month;
    st.wDayOfWeek = (WORD) dt.dayOfWeek;
    st.wDay = (WORD) dt.day;
    st.wHour = (WORD) dt.hour;
    st.wMinute = (WORD) dt.minute;
    st.wSecond = (WORD) dt.second;
    st.wMilliseconds = (WORD) dt.milliseconds;
    return st;
}

__int64 DateTimeToInt( const DateTime& dt )
{
    SYSTEMTIME st = DateTimeToSysTime( dt );

    FILETIME ft;
    SystemTimeToFileTime( &st, &ft );

    ULARGE_INTEGER    large;

    large.LowPart  = ft.dwLowDateTime;
    large.HighPart = ft.dwHighDateTime;

    return large.QuadPart;
}

DateTime IntToDateTime( __int64 i )
{
    ULARGE_INTEGER large;
    large.QuadPart = i;

    FILETIME ft;
    ft.dwLowDateTime = large.LowPart;
    ft.dwHighDateTime = large.HighPart;

    SYSTEMTIME st;
    FileTimeToSystemTime( &ft, &st );

    return SysTimeToDateTime( st );
}


}

DateTime DateTime::GetLocalTime()
{
    SYSTEMTIME lt;

    ::GetLocalTime( &lt );

    return SysTimeToDateTime( lt );
}

void DateTime::AddSeconds( double seconds )
{
    __int64 i = DateTimeToInt( *this );

    i += (__int64) ( seconds * (double) _SECOND );

    *this = IntToDateTime( i );
    return;
}

bool DateTime::IsInRange( const DateTime& start, const DateTime& end ) const
{
    __int64 i = DateTimeToInt( *this );
    __int64 startI = DateTimeToInt( start );
    __int64 endI = DateTimeToInt( end );

    return i >= startI && i <= endI;
}

double DateTime::SecondsSince( const DateTime& sinceThisTime ) const
{
    __int64 i = DateTimeToInt( *this );
    __int64 sinceI = DateTimeToInt( sinceThisTime );
    return (double) ( (double) ( i - sinceI ) / (double) _SECOND );
}


string DateTime::ToString()
{
    return Stringf( "%d/%02d/%02d %02d:%02d:%02d %3dms",
                    year, month, day, hour, minute, second, milliseconds );
}

string DateTime::ToStringTimeOnly()
{
    return Stringf( "%02d:%02d:%02d %3dms",
                    hour, minute, second, milliseconds );
}

string DateTime::ToStringFileFriendly()
{
    return Stringf( "%d.%02d.%02d_%02d.%02d.%02d %3d",
                    year, month, day, hour, minute, second, milliseconds );
}

bool DateTime::operator>( const DateTime& rhs ) const
{
    __int64 i = DateTimeToInt( *this );
    __int64 rhsI = DateTimeToInt( rhs );
    return i > rhsI;
}

bool DateTime::operator>=( const DateTime& rhs ) const
{
    __int64 i = DateTimeToInt( *this );
    __int64 rhsI = DateTimeToInt( rhs );
    return i >= rhsI;
}

bool DateTime::operator<( const DateTime& rhs ) const
{
    return !( *this >= rhs );
}

bool DateTime::operator<=( const DateTime& rhs ) const
{
    return !( *this > rhs );
}

bool DateTime::operator!=( const DateTime& rhs ) const
{
    return !( *this == rhs );
}

bool DateTime::operator==( const DateTime& rhs ) const
{
    __int64 i = DateTimeToInt( *this );
    __int64 rhsI = DateTimeToInt( rhs );
    return i == rhsI;
}
