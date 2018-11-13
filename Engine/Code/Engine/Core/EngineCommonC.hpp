#pragma once

#include "Engine/String/StringUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommonH.hpp"


#include <functional>
#include <limits.h>



void CallFunctinoNTimes( std::function<void()> func, int timesToCall );

constexpr size_t MAX_SIZE_T = std::numeric_limits<size_t>::max();