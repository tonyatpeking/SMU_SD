#pragma once
#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/IVec3.hpp"
#include "Engine/Math/RaycastHit3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Game/GameState.hpp"
#include "Game/GameplayEnums.hpp"

class Menu;
class ShaderProgram;
class Mesh;
class TextMeshBuilder;
class Renderalbe;
class GameObject;
class Light;
class Asteroid;
class Projectile;
class ParticleEmitter;
class Image;
class GameMap;
class Hive;
class Swarmer;

class GameState_Playing : public GameState
{
public:
    GameState_Playing();
    ~GameState_Playing() override;
    void Update() override;
    void Render() const override;
    void OnEnter() override;
    void OnExit() override;
    void ProcessInput() override;

private:

    void MakeCamera();
    void ProcessMovementInput();

    void UpdateCameraToFollow();

    void MakeSun();
    void SetAmbient( float ambient );

    void BuildShipFromTree();

    // Ship
    GameObject* m_ship = nullptr;
    float m_rollSpeed = 100.f;
    float m_cameraRotateSpeed = 0.3f;

    Light* m_sun = nullptr;

    Vec3 m_lightPos;
    uint m_lightDebugHandle = 0;
    uint m_lightsToSpawn = 1;

    float m_lightSourceRadius = 0.2f;
    float m_lightIntensity = 400;
    float m_projectileVelocity = 50;

    float m_camYaw = -90;
    float m_camPitch = 0;

    float m_fov = 80;
    float m_near = 1.f;
    float m_far = 300;


};