#pragma once

#include <string>
#include "Engine/String/ParseStatus.hpp"
#include "Engine/Core/Types.hpp"

class IRange
{
public:
    IRange() {};
    ~IRange() {};

    IRange( const IRange& copyFrom );
    explicit IRange( int minInclusive, int maxInclusive );
    explicit IRange( int minMaxInclusive );

    ParseStatus SetFromText( const String& text, const String& delimiter = "~" );

    int GetRandomInRange() const;
    bool IsInRange( int val ) const;

    // operators
    bool operator==( const IRange& compare ) const;
    IRange operator*( int scale ) const;

    // static methods
    static bool DoRangesOverlap( const IRange& a, const IRange& b );
public:
    int min = 0;
    int max = 0;
};

const IRange Lerp( const IRange& start, const IRange& end, float t );