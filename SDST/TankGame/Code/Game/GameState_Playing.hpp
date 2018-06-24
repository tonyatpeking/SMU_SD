#pragma once
#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/IVec3.hpp"
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


    //debug assignment
    void MakePrimitiveShapes();
    std::vector<Asteroid*> m_asteroids;
    std::vector<Projectile*> m_projectiles;
    std::vector<ParticleEmitter*> m_emitters;
    std::vector<Light*> m_lights;
    GameObject* m_shipCenter;
    GameObject* m_miku;


    //tests
    void TestEnter();
    void TestRender() const;
    void TestUpdate();
    void TestInput();

    void UpdateCameraToFollow();
    void LoadMiku();
    void MakeLights();
    void FinalizeSpaceShip();
    void UpdateLights();
    void RenderLights() const;
    void MakeSun();
    void SetLightPos( const Vec3& pos );
    void SetAmbient( float ambient );

    void MakeTurret();
    void MakeProjectile( const Vec3& offset );

    void MakeAsteroids();
    void MakeAsteroid( float maxHealth, const Vec3& position );
    void CheckCollisions();
    void MakeCollisionParticles( const Vec3& position );
    ParticleEmitter* MakeExhaustEmitter( const Vec3& offset );
    void SetExhaustSpawnRate( float rate );
    void SnapTransformToHeightmap( Transform* transform );
    void LeaveBreadCrumbs();

    void UpdateRaycastHitIndicator();


    GameObject* m_raycastHitIndicator = nullptr;
    GameObject* m_turret = nullptr;


    ParticleEmitter* m_leftThrustEmitter;
    ParticleEmitter* m_rightThrustEmitter;

    Vec3 m_lightPos;
    uint m_lightDebugHandle = 0;
    uint m_lightsToSpawn = 18;
    float m_lightSourceRadius = 0.2f;
    float m_lightIntensity = 400;
    float m_projectileVelocity = 50;

    float m_camYaw = -90;
    float m_camPitch = 0;

    float m_fov = 80;
    float m_near = 0.001f;
    float m_far = 1000;

    float m_moveSpeed = 15;
    float m_rollSpeed = 200;
    float m_turnSpeed = 0.2f;

};