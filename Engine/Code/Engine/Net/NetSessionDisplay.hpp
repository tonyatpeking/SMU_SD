#pragma once
#include "Engine/Math/AABB2.hpp"

class TextRenderable;

class NetSessionDisplay
{
public:
    static NetSessionDisplay* GetDefault();
	NetSessionDisplay();
	~NetSessionDisplay();

    void Render();

private:

    TextRenderable* m_titleText = nullptr;
    TextRenderable* m_infoText = nullptr;
    AABB2 m_bounds;
};
