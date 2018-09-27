#include "Engine/Math/IRange.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"

IRange::IRange( const IRange& copyFrom )
    : min( copyFrom.min )
    , max( copyFrom.max )
{

}

IRange::IRange( int minInclusive, int maxInclusive )
    : min( minInclusive )
    , max( maxInclusive )
{

}

IRange::IRange( int minMaxInclusive )
    : min( minMaxInclusive )
    , max( minMaxInclusive )
{

}

ParseStatus IRange::SetFromText( const string& text, const string& delimiter )
{

    vector<int> out_ints;
    ParseStatus status = StringUtils::ParseToInts( text, out_ints, delimiter );

    //ASSERT_RECOVERABLE( out_ints.size() == 1 || out_ints.size() == 2, "Wrong number of elements passed to IRange" );

    if( out_ints.size() == 1 )
    {
        min = out_ints[0];
        max = out_ints[0];
        return status;
    }
    else if( out_ints.size() == 2 )
    {
        min = out_ints[0];
        max = out_ints[1];
        return status;
    }

    if( status == PARSE_SUCCESS )
        return PARSE_WRONG_NUMBER_OF_TOKENS;

    return status;
}

int IRange::GetRandomInRange() const
{
    return Random::IntInRange( min, max );
}

bool IRange::IsInRange( int val ) const
{
    return ( min <= val ) && ( val <= max );
}

bool IRange::operator==( const IRange& compare ) const
{
    return( min == compare.min ) && ( max == compare.max );
}

IRange IRange::operator*( int scale ) const
{
    return IRange( min * scale, max * scale );
}

bool IRange::DoRangesOverlap( const IRange& a, const IRange& b )
{
    return a.IsInRange( b.min ) || a.IsInRange( b.max );
}

const IRange Lerp( const IRange& start, const IRange& end, float t )
{
    int newMin = Lerp( start.min, end.min, t );
    int newMax = Lerp( start.max, end.max, t );
    return IRange( newMin, newMax );
}
