#pragma once
#include "Engine/Core/EngineCommon.hpp"


namespace ShapeRulesetLoader
{

void Init();

void Load( const String& file );

bool DidCurrentRuleChange();


};
