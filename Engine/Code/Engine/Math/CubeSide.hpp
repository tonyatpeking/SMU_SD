#pragma once

#include "Engine/Core/SmartEnum.hpp"


SMART_ENUM(
    CubeSide,

    INVALID = -1,
    RIGHT,
    LEFT,
    TOP,
    BOTTOM,
    BACK,
    FRONT
)

class IVec3;

IVec3 CubeSideToNormal( CubeSide side );
CubeSide NormalToCubeSide( const IVec3& normal );
