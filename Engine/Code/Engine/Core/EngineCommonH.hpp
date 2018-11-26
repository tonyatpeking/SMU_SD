#pragma once
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

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
constexpr size_t MAX_SIZE_T = (size_t) ( ~0 );

// math constants
constexpr float M_SQRT2_OVER_2 = (float) M_SQRT2 / 2.f;
constexpr float EPSILON = 0.000001f;


#define UNUSED(x) (void)(x);


#define BIT_FLAG(f)   (1U << (f))




