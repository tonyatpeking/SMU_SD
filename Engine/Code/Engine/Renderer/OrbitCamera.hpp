#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Vec3.hpp"

class OrbitCamera : public Camera
{
public:
	OrbitCamera();
	~OrbitCamera(){};

    void SetTarget( const Vec3& target );
    void SetSphericalCoord( float radius, float elevation, float azimuth );
public:

    Vec3 m_targetPos;
    float m_radius = 0.f;
    float m_elevation = 0.f;
    float m_azimuth = 0.f;
private:

    void CalculateMatrices();
};
