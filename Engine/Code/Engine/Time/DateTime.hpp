#pragma once
#include "Engine/Core/Types.hpp"

class DateTime
{
public:

    static DateTime GetLocalTime();

    void AddSeconds( double seconds );

    bool IsInRange( const DateTime& startInclusive, const DateTime& endInclusive ) const;

    // can be zero or negative if sinceThisTime is in the future
    double SecondsSince( const DateTime& sinceThisTime ) const;

    string ToString();
    string ToStringTimeOnly();
    string ToStringFileFriendly();

    bool operator>( const DateTime& rhs ) const;
    bool operator>=( const DateTime& rhs ) const;
    bool operator<( const DateTime& rhs ) const;
    bool operator<=( const DateTime& rhs ) const;
    bool operator==( const DateTime& rhs ) const;
    bool operator!=( const DateTime& rhs ) const;


public:

    int year = 0;
    int month = 0;
    int dayOfWeek = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    int milliseconds = 0;

};