#pragma once

#include <string>
#include "Engine/String/ParseStatus.hpp"
#include "Engine/Core/Types.hpp"

class Range
{
public:
    Range() {};
    ~Range() {};

    Range( const Range& copyFrom );
    explicit Range( float minInclusive, float maxInclusive );
    explicit Range( float minMaxInclusive );

    ParseStatus SetFromText( const string& text, const string& delimiter = "~" );

    bool IsInRange( float val ) const;

    // static
    static bool DoRangesOverlap( const Range& a, const Range& b );
public:
    float min = 0.f;
    float max = 0.f;
};

const Range Lerp( const Range& start, const Range& end, float t );