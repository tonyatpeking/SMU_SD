#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
class XMLElement;


class Texture;


class Sprite
{
public:
	Sprite(){};
	~Sprite(){};

public:
    Vec2 m_pivot;
    Vec2 m_dimensions;
    Texture* m_texture;
    AABB2 m_uvs;
    float m_pixelsPerUnit;
};

