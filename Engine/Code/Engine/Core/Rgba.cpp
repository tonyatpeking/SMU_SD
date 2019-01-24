#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"


// Color wheel
const Rgba Rgba::RED           =		Rgba( 255, 0, 0, 255 );
const Rgba Rgba::ORANGE        =		Rgba( 255, 128, 0, 255 );
const Rgba Rgba::YELLOW        =		Rgba( 255, 255, 0, 255 );
const Rgba Rgba::GREEN_YELLOW  =		Rgba( 128, 255, 0, 255 );
const Rgba Rgba::GREEN         =		Rgba( 0, 255, 0, 255 );
const Rgba Rgba::GREEN_CYAN    =		Rgba( 0, 255, 128, 255 );
const Rgba Rgba::CYAN          =		Rgba( 0, 255, 255, 255 );
const Rgba Rgba::BLUE_CYAN     =		Rgba( 0, 128, 255, 255 );
const Rgba Rgba::BLUE          =		Rgba( 0, 0, 255, 255 );
const Rgba Rgba::BLUE_MAGENTA  =		Rgba( 128, 0, 255, 255 );
const Rgba Rgba::MAGENTA       =		Rgba( 255, 0, 255, 255 );
const Rgba Rgba::RED_MAGENTA   =		Rgba( 255, 0, 128, 255 );

// Colors I know
const Rgba Rgba::PINK          =		Rgba( 255, 192, 203, 255 );
const Rgba Rgba::BROWN         =		Rgba( 128, 64, 0, 255 );
const Rgba Rgba::PURPLE        =		Rgba( 128, 0, 128, 255 );
const Rgba Rgba::VIOLET        =		Rgba( 127, 0, 255, 255 );
const Rgba Rgba::OLIVE         =		Rgba( 128, 128, 0, 255 );

// Gray scale and clears
const Rgba Rgba::CLEAR_WHITE   =		Rgba( 255, 255, 255, 0 );
const Rgba Rgba::CLEAR_BLACK   =		Rgba( 0, 0, 0, 0 );
const Rgba Rgba::WHITE         =		Rgba( 255, 255, 255, 255 );
const Rgba Rgba::BLACK         =		Rgba( 0, 0, 0, 255 );
const Rgba Rgba::LIGHT_GRAY    =		Rgba( 211, 211, 211, 255 );
const Rgba Rgba::GRAY          =		Rgba( 128, 128, 128, 255 );
const Rgba Rgba::DIM_GRAY      =		Rgba( 105, 105, 105, 255 );




Rgba::Rgba()
    : r( 255 ), g( 255 ), b( 255 ), a( 255 ) //default color is white
{
}

Rgba::Rgba( const Rgba & copy )
    : r( copy.r ), g( copy.g ), b( copy.b ), a( copy.a )
{
}

Rgba Rgba::operator+( const Rgba& toAdd )
{
    Rgba added;
    added.r = r + toAdd.r;
    added.g = g + toAdd.g;
    added.b = b + toAdd.b;
    added.a = a + toAdd.a;
    return added;
}

Rgba Rgba::operator*( float scale )
{
    Rgba scaled;
    scaled.r = (uchar) ( (float) r * scale );
    scaled.g = (uchar) ( (float) g * scale );
    scaled.b = (uchar) ( (float) b * scale );
    scaled.a = (uchar) ( (float) a * scale );
    return scaled;
}

void Rgba::operator=( const Rgba & copyFrom )
{
    r = copyFrom.r;
    g = copyFrom.g;
    b = copyFrom.b;
    a = copyFrom.a;
}

Rgba::Rgba(
    unsigned char redByte,
    unsigned char greenByte,
    unsigned char blueByte,
    unsigned char alphaByte )
    : r( redByte )
    , g( greenByte )
    , b( blueByte )
    , a( alphaByte )
{
}

bool Rgba::operator==( const Rgba& compare ) const
{
    return (
        r == compare.r
        && g == compare.g
        && b == compare.b
        && a == compare.a
        );
}

void Rgba::SetAsBytes( unsigned char redByte, unsigned char greenByte,
                       unsigned char blueByte, unsigned char alphaByte )
{
    r = redByte;
    g = greenByte;
    b = blueByte;
    a = alphaByte;
}

void Rgba::SetAsFloats( float normalizedRed, float normalizedGreen,
                        float normalizedBlue, float normalizedAlpha )
{
    r = (unsigned char) ( normalizedRed * 255.f );
    g = (unsigned char) ( normalizedGreen * 255.f );
    b = (unsigned char) ( normalizedBlue * 255.f );
    a = (unsigned char) ( normalizedAlpha * 255.f );
}

ParseStatus Rgba::SetFromText( const string& text, const string& delimiter )
{
    vector<int> out_ints;
    ParseStatus status = StringUtils::ParseToInts( text, out_ints, delimiter );

    if( out_ints.size() != 3 && out_ints.size() != 4 )
        return PARSE_WRONG_NUMBER_OF_TOKENS;

    if( out_ints.size() == 3 )
    {
        r = (unsigned char) out_ints[0];
        g = (unsigned char) out_ints[1];
        b = (unsigned char) out_ints[2];
        a = 255;
    }
    else if( out_ints.size() == 4 )
    {
        r = (unsigned char) out_ints[0];
        g = (unsigned char) out_ints[1];
        b = (unsigned char) out_ints[2];
        a = (unsigned char) out_ints[3];
    }

    return status;
}

void Rgba::GetAsFloats( float& out_normalizedRed, float& out_normalizedGreen,
                        float& out_normalizedBlue, float& out_normalizedAlpha ) const
{
    out_normalizedRed = ( (float) r ) / 255.f;
    out_normalizedGreen = ( (float) g ) / 255.f;
    out_normalizedBlue = ( (float) b ) / 255.f;
    out_normalizedAlpha = ( (float) a ) / 255.f;
}

void Rgba::GetAsFloats( float* out_floats ) const
{
    GetAsFloats( out_floats[0], out_floats[1], out_floats[2], out_floats[3] );
}

Vec4 Rgba::ToVec4() const
{
    return Vec4(
        ( (float) r ) / 255.f,
        ( (float) g ) / 255.f,
        ( (float) b ) / 255.f,
        ( (float) a ) / 255.f );
}

Vec3 Rgba::ToVec3() const
{
    return Vec3(
        ( (float) r ) / 255.f,
        ( (float) g ) / 255.f,
        ( (float) b ) / 255.f );
}

vector<Rgba>& Rgba::GetColorWheel()
{
    static vector<Rgba> s_colorWheel ={
    RED
    ,ORANGE
    ,YELLOW
    ,GREEN_YELLOW
    ,GREEN
    ,GREEN_CYAN
    ,CYAN
    ,BLUE_CYAN
    ,BLUE
    ,BLUE_MAGENTA
    ,MAGENTA
    ,RED_MAGENTA };
    return s_colorWheel;
}

void Rgba::ScaleRGB( float rgbScale )
{
    r = (unsigned char) Clampf( (float) r * rgbScale, 0.f, 255.f );
    g = (unsigned char) Clampf( (float) g * rgbScale, 0.f, 255.f );
    b = (unsigned char) Clampf( (float) b * rgbScale, 0.f, 255.f );
}

void Rgba::ScaleAlpha( float alphaScale )
{
    a = (unsigned char) Clampf( (float) a * alphaScale, 0.f, 255.f );
}

Rgba Lerp( const Rgba& start, const Rgba& end, float t )
{
    unsigned char r = Lerp( start.r, end.r, t );
    unsigned char g = Lerp( start.g, end.g, t );
    unsigned char b = Lerp( start.b, end.b, t );
    unsigned char a = Lerp( start.a, end.a, t );
    return Rgba( r, g, b, a );
}

bool operator < ( const Rgba &A, const Rgba &B )
{
    if( A.r < B.r )  return true;
    if( A.r > B.r )  return false;

    if( A.g < B.g )  return true;
    if( A.g > B.g )  return false;

    if( A.b < B.b )  return true;
    if( A.b > B.b )  return false;

    if( A.a < B.a )  return true;
    if( A.a > B.a )  return false;

    // Otherwise both are equal
    return false;
}