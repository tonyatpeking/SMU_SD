#pragma once

#include "Engine/String/StringUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommonH.hpp"


#include <functional>



void CallFunctinoNTimes( std::function<void()> func, int timesToCall );

