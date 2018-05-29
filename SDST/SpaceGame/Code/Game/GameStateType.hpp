#pragma once
#include "Engine/Core/SmartEnum.hpp"

SMART_ENUM
(
    GameStateType,

    INVALID = -1,
    PLAYING,
    LOADING,
    MAIN_MENU,
    VICTORY
)