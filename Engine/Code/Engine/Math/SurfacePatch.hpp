#pragma once
#include "Engine/Core/SmartEnum.hpp"
#include "Engine/Math/MathUtils.hpp"
class Vec2;
class Vec3;
class MeshBuilder;
class Mesh;

class SurfacePatch
{
public:

    SurfacePatch() {};
    virtual ~SurfacePatch() {};

    Vec3 EvalPosition( const Vec2& uv );
    Vec3 EvalTangent( const Vec2& uv );
    Vec3 EvalBitangent( const Vec2& uv );
    Vec3 EvalNormal( const Vec2& uv );
    Vec3 CalcNormal( const Vec3& bitangent, const Vec3& tangent );

    Wrap::WrapMode m_wrapMode = Wrap::WrapMode::CLAMP;
    float m_eps = 0.00001f;
protected:

    virtual Vec3 EvalImpl( const Vec2& uv ) = 0;

private:

};


//--------------------------------------------------------------------------------------
// Cylinder

// cylinder does not have caps
// radius is 1, height is one
class SurfacePatch_Cylinder : public SurfacePatch
{
protected:
    virtual Vec3 EvalImpl( const Vec2& uv ) override;
};

//--------------------------------------------------------------------------------------
// Cone

// Cone does not have caps
// radius of bottom is 1, height is one
class SurfacePatch_Cone : public SurfacePatch
{
protected:
    virtual Vec3 EvalImpl( const Vec2& uv ) override;
};


//--------------------------------------------------------------------------------------
// Helicoid

// radius of bottom is 1, height is one
class SurfacePatch_Helicoid : public SurfacePatch
{
public:
    // alpha controls curvature
    SurfacePatch_Helicoid( float alpha = 1 )
        : m_alpha( alpha ) {};
protected:
    virtual Vec3 EvalImpl( const Vec2& uv ) override;
    float m_alpha = 1;
};


//--------------------------------------------------------------------------------------
// Torus

class SurfacePatch_Torus : public SurfacePatch
{
public:
    // alpha controls curvature
    SurfacePatch_Torus( float tubeRadius = 0.4f, float loopRadius = 1.f )
        : m_tubeRadius( tubeRadius )
        , m_loopRadius( loopRadius )
    {};
protected:
    virtual Vec3 EvalImpl( const Vec2& uv ) override;
    float m_tubeRadius;
    float m_loopRadius;
};