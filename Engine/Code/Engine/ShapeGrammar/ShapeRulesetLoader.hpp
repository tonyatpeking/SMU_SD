#pragma once
#include "Engine/Core/EngineCommonH.hpp"


namespace ShapeRulesetLoader
{

void Init();

void Load( const string& file );

bool DidCurrentRuleChange();


};
