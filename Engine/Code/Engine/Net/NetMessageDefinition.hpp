#pragma once
#include "Engine/Core/EngineCommonH.hpp"

class NetMessageDefinition
{
public:
	NetMessageDefinition(){};
	~NetMessageDefinition(){};

private:

    string m_name;

    uint m_idx = (uint) -1;

};
