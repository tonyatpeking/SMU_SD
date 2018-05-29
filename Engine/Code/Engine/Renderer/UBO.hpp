#pragma once
#include "Engine/Math/Mat4.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "RendererEnums.hpp"

namespace UBO
{
constexpr int TIME_BINDING = 1;
constexpr int CAMERA_BINDING = 2;
constexpr int INSTANCE_BINDING = 3;
constexpr int USER_BINDING = 4;
constexpr int GLOBAL_BINDING = 5;
constexpr int LIGHT_BINDING = 6;




struct TimeData
{
    float gameDeltaSeconds = 0;
    float gameSeconds = 0;
    float appDeltaSeconds = 0;
    float appSeconds = 0;
};

struct CameraData
{
    Mat4 view;
    Mat4 projection;
    Mat4 vp;
    Vec3 cameraPosition;
    float padCam0;
};

struct InstanceData
{
    Mat4 model;
    Mat4 mvp;
    Vec4 tint;
    float specularAmount;
    float specularPower;
    float padInst0;
    float padInst1;
};

struct GlobalData
{
    Vec2 windowSize;
};

struct Light
{
    Vec4 color; // alpha is intensity

    Vec3 position;
    float isPointLight;  // 1 for point light, 0 for directional light

    Vec3 direction; // the direction the light is pointing
    float sourceRadius; // the physical size of the light, affects attenuation

    float coneInnerDot;
    float coneOuterDot;
    float padLight1;
    float padLight0;

};

struct LightsData
{
    Vec4 ambient;
    Light lights[MAX_LIGHTS];
};



}