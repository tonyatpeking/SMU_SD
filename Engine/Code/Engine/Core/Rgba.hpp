#pragma once
#include <string>
#include "Engine/String/ParseStatus.hpp"
#include "Engine/Core/Types.hpp"


class Vec3;
class Vec4;

class Rgba
{
public:
    // Constructors / destructors:
    ~Rgba() {}
    Rgba();
    Rgba( const Rgba& copy );
    explicit Rgba( unsigned char redByte, unsigned char greenByte,
                   unsigned char blueByte, unsigned char alphaByte = 255 );

    // Operators
    Rgba operator+( const Rgba& toAdd );
    Rgba operator*( float scale );
    void operator=( const Rgba& copyFrom );
    bool operator==( const Rgba& compare ) const;
    friend bool operator<( const Rgba& a, const Rgba& b );

    // Mutators
    void SetAsBytes( unsigned char redByte, unsigned char greenByte,
                     unsigned char blueByte, unsigned char alphaByte=255 );

    void SetAsFloats( float normalizedRed, float normalizedGreen, float normalizedBlue,
                      float normalizedAlpha=1.0f );

    ParseStatus SetFromText( const String& text, const String& delimiter = "," );
    void ScaleRGB( float rgbScale ); // Scales (and clamps) RGB components, but not A
    void ScaleAlpha( float alphaScale ); // Scales (and clamps) Alpha, RGB is untouched

    // Accessors
    void GetAsFloats( float& out_normalizedRed, float& out_normalizedGreen,
                      float& out_normalizedBlue, float& out_normalizedAlpha ) const;
    void GetAsFloats(float* out_floats) const;
    Vec4 ToVec4() const;

    // Ignores alpha
    Vec3 ToVec3() const;

public:
    unsigned char r = 255;
    unsigned char g = 0;
    unsigned char b = 255;
    unsigned char a = 255;

    // Color wheel
    static const Rgba RED;
    static const Rgba ORANGE;
    static const Rgba YELLOW;
    static const Rgba GREEN_YELLOW;
    static const Rgba GREEN;
    static const Rgba GREEN_CYAN;
    static const Rgba CYAN;
    static const Rgba BLUE_CYAN;
    static const Rgba BLUE;
    static const Rgba BLUE_MAGENTA;
    static const Rgba MAGENTA;
    static const Rgba RED_MAGENTA;

    // Colors I know
    static const Rgba PINK;
    static const Rgba BROWN;
    static const Rgba PURPLE;
    static const Rgba VIOLET;
    static const Rgba OLIVE;

    // Gray scale and clears
    static const Rgba CLEAR_WHITE;
    static const Rgba CLEAR_BLACK;
    static const Rgba WHITE;
    static const Rgba BLACK;
    static const Rgba LIGHT_GRAY;
    static const Rgba GRAY;
    static const Rgba DIM_GRAY;

};

Rgba Lerp( const Rgba& start, const Rgba& end, float t );