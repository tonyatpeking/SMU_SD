#pragma once
#include <functional>
#include "Engine/Core/EngineCommon.hpp"

namespace Solver
{
bool Quadratic( float a, float b, float c, float& out_root1, float& out_root2 ); // root1 <= root2
float BinarySearch( float start, float end,
                   ::std::function<float( float t )> eval,
                   int iterations = 10, float eps = EPSILON );
}