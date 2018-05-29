#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/VertexLayout.hpp"

struct VertexBuilderData
{
    Vec3 m_position;
    Rgba m_color;
    Vec2 m_uv;

    Vec3 m_normal;
    Vec4 m_tangent; // 4th element is 1/-1 to indicate whether bi-tangent is flipped
};

struct VertexPCU
{
    VertexPCU() {};
    VertexPCU( const Vec3& pos, const Rgba& color = Rgba::WHITE,
               const Vec2& uv = Vec2::ZEROS )
        : m_position( pos )
        , m_color( color )
        , m_uv( uv ) {};

    Vec3 m_position;
    Rgba m_color;
    Vec2 m_uv;

    static const VertexAttribute s_attributes[];
    static void s_copier( void *dst, const VertexBuilderData* src, uint count );
    static const VertexLayout s_vertexLayout;
};

struct VertexLit
{
    Vec3 m_position;
    Rgba m_color;
    Vec2 m_uv;

    Vec3 m_normal;
    Vec4 m_tangent; // 4th element is 1/-1 to indicate whether bi-tangent is flipped

    static const VertexAttribute s_attributes[];
    static void s_copier( void *dst, const VertexBuilderData* src, uint count );
    static const VertexLayout s_vertexLayout;
};

