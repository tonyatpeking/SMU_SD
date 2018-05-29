#pragma once
#include <functional>
#include <vector>
#include <string>

#include "Engine/Core/Types.hpp"

enum class ErrorStatus
{
    SUCCESS,
    FAIL
};

// file paths
constexpr char* FONT_PATH = "Data/Fonts/";

// type sizes
constexpr short MAX_SHORT = 32767;
constexpr float INVERSE_MAX_SHORT = 1.f / (float) MAX_SHORT;
constexpr unsigned char MAX_UCHAR = 255;
constexpr float INVERSE_MAX_UCHAR = 1.f / (float) MAX_UCHAR;

// math constants
constexpr float M_PI = 3.14159265359f;
constexpr float M_SQRT2 = 1.41421356237f;
constexpr float M_SQRT2_OVER_2 = M_SQRT2 / 2.f;
constexpr float EPS = 0.000001f;


#define UNUSED(x) (void)(x);

void CallFunctinoNTimes( std::function<void()> func, int timesToCall );



