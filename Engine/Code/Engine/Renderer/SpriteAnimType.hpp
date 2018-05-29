#pragma once

#include <string>

#include "Engine/Core/SmartEnum.hpp"

class XMLElement;
class IVec2;

// The direction variants must be in the order north, west, south, east
SMART_ENUM(
    SpriteAnimType,

    INVALID,

    IDLE,
    IDLE_NORTH,
    IDLE_WEST,
    IDLE_SOUTH,
    IDLE_EAST,

    CAST,
    CAST_NORTH,
    CAST_WEST,
    CAST_SOUTH,
    CAST_EAST,

    THRUST,
    THRUST_NORTH,
    THRUST_WEST,
    THRUST_SOUTH,
    THRUST_EAST,

    MOVE,
    MOVE_NORTH,
    MOVE_WEST,
    MOVE_SOUTH,
    MOVE_EAST,

    SLASH,
    SLASH_NORTH,
    SLASH_WEST,
    SLASH_SOUTH,
    SLASH_EAST,

    SHOOT,
    SHOOT_NORTH,
    SHOOT_WEST,
    SHOOT_SOUTH,
    SHOOT_EAST,

    LAY_DOWN
)

SpriteAnimType SpriteAnimTypeWithDirection( SpriteAnimType baseAnim, const IVec2& direction );

// This returns the SpriteAnimType from a string in xml
const SpriteAnimType ParseXmlAttribute( const XMLElement& element, const char* attributeName, const SpriteAnimType defaultValue );
