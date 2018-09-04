#include "Engine/Math/SurfacePatch.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Image/Image.hpp"


Vec3 SurfacePatch::EvalPosition( const Vec2& uv )
{
    return EvalImpl( Wrap::WrapUV01( uv, m_wrapMode ) );
}

Vec3 SurfacePatch::EvalTangent( const Vec2& uv )
{
    Vec3 posRight = EvalPosition( uv + Vec2( m_eps, 0.f ) );
    Vec3 posLeft = EvalPosition( uv - Vec2( m_eps, 0.f ) );
    return ( posRight - posLeft ).GetNormalized();
}

Vec3 SurfacePatch::EvalBitangent( const Vec2& uv )
{
    Vec3 posUp = EvalPosition( uv + Vec2( 0.f, m_eps ) );
    Vec3 posDown = EvalPosition( uv - Vec2( 0.f, m_eps ) );
    return ( posUp - posDown ).GetNormalized();
}

Vec3 SurfacePatch::EvalNormal( const Vec2& uv )
{
    Vec3 tangent = EvalTangent( uv );
    Vec3 bitangent = EvalBitangent( uv );
    return CalcNormal( bitangent, tangent );
}

Vec3 SurfacePatch::CalcNormal( const Vec3& bitangent, const Vec3& tangent )
{
    return Cross( bitangent, tangent );
}



//--------------------------------------------------------------------------------------
// Cylinder

Vec3 SurfacePatch_Cylinder::EvalImpl( const Vec2& uv )
{
    float x, y, z;
    y = uv.v;
    float theta = RangeMapFloat( uv.u, 0.f, 1.f, 0.f, 360.f );
    x = CosDeg( theta );
    z = SinDeg( theta );
    return Vec3( x, y, z );
}

//--------------------------------------------------------------------------------------
// Cone

Vec3 SurfacePatch_Cone::EvalImpl( const Vec2& uv )
{
    float x, y, z;
    y = uv.v;
    float theta = RangeMapFloat( uv.u, 0.f, 1.f, 0.f, 360.f );
    float radius = 1.f - y;
    x = radius * CosDeg( theta );
    z = radius * SinDeg( theta );
    return Vec3( x, y, z );
}

//--------------------------------------------------------------------------------------
// Cone

Vec3 SurfacePatch_Helicoid::EvalImpl( const Vec2& uv )
{
    //from https://en.wikipedia.org/wiki/Helicoid
    float x, y, z;
    float theta = RangeMapFloat( uv.v, 0.f, 1.f, 0.f, 360.f );
    float rho = RangeMapFloat( uv.u, 0.f, 1.f, -1.f, 1.f );
    y = uv.v;

    x = rho * CosDeg( theta * m_alpha );
    z = rho * SinDeg( theta * m_alpha );
    return Vec3( x, y, z );
}

//--------------------------------------------------------------------------------------
// Torus
Vec3 SurfacePatch_Torus::EvalImpl( const Vec2& uv )
{
    //from https://en.wikipedia.org/wiki/Torus
    float x, y, z;
    float theta = RangeMapFloat( uv.v, 0.f, 1.f, 0.f, 360.f );
    float phi = RangeMapFloat( uv.u, 0.f, 1.f, 0.f, 360.f );

    x = ( m_loopRadius + m_tubeRadius * CosDeg( theta ) ) * CosDeg( phi );
    z = ( m_loopRadius + m_tubeRadius * CosDeg( theta ) ) * SinDeg( phi );
    y = m_tubeRadius * SinDeg( theta );
    return Vec3( x, y, z );

}

//--------------------------------------------------------------------------------------
// HeightMap
Vec3 SurfacePatch_HeightMap::EvalImpl( const Vec2& uv )
{
    float x, y, z;
    Vec2 posXZ = RangeMap( uv, Vec2::ZEROS, Vec2::ONES, m_extents.mins, m_extents.maxs );
    x = posXZ.x;
    z = posXZ.y;

        float red = (float) m_heightMap->GetTexelAtUV( uv ).r;
        y = RangeMap( red, 0.f, 255.f, m_minHeight, m_maxHeight );


    return Vec3( x,y,z );
}


