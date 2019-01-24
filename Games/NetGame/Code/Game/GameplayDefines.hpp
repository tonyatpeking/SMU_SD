#pragma once
#include "Engine/Core/SmartEnum.hpp"


#define PLAYER_MOVE_SPEED (10.f)
#define BULLET_SPEED (20.f)
#define BULLET_LIFETIME (0.4f)
#define PLAYER_SHOOT_INTERVAL (0.2f)
#define PLAYER_BULLET_COLLISION_DIST_SQUARED (1*1)
#define BULLET_COLOR_BLEND_WEIGHT (0.2f) // higher value means bullet gets more weight
#define VICTORY_COLOR_DEVIATION (30.f)

#define ADDITIONAL_COMMAND_LINE_ARGS ("")

// runtime vars
#define TILE_WINDOW ("tile_window")
#define AUTO_JOIN ("auto_join")