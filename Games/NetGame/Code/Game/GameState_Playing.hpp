#pragma once
#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/IVec3.hpp"
#include "Engine/Math/RaycastHit3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Game/GameState.hpp"
#include "Game/GameplayDefines.hpp"
#include "Game/ClientInputs.hpp"

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
class NetCube;
class NetSession;
class Player;

class GameState_Playing : public GameState
{
public:
    static GameState_Playing* GetDefault();
    GameState_Playing();
    ~GameState_Playing() override;
    void Update() override;
    void HostUpdate();
    void ClientUpdate();
    void Render() const override;
    void OnEnter() override;
    void OnExit() override; 
    void ProcessInput() override;

    // Host
    void Process_EnterGame( uint8 playerID );
    void SendCreateCube( uint8 playerID, NetCube* cube ); // skips if receiver is host
    void SendDestroyCube( uint16 netID );  // skips if receiver is host
    void SendCreateCubeToAll( NetCube* cube ); // skips if receiver is host
    void CreatePlayerCube( uint8 playerID );
    void SendCubeUpdatesForAllClients();
    void CreateBulletForPlayer( uint8 playerID );
    void RemoveDisconnectedPlayers();
    void CheckForVictoryReset();
    NetCube* GetPlayerCube( uint8 playerID );
    Player* GetPlayer( uint8 playerID );

    // Host gameplay
    void Process_SendInputs( uint8 playerID, const ClientInputs& inputs );
    void UpdatePlayerInputs();
    void UpdateBullets();

    // Client
    void SendInputsToHost();
    void Process_CreateCube( const Vec3& position,
                             const Vec3& velocity,
                             const Vec3& scale,
                             const Rgba& color,
                             uint16 netID );
    void SendEnterGame();
    void Process_DestroyCube(uint16 netID);
    void Process_UpdateCube( const Vec3& position,
                             const Rgba& color,
                             uint16 netID );

    bool IsHost();

private:

    NetSession* m_session;

    // Host
    map<uint8, Player*> m_players;

//     map<uint8,NetCube*> m_playerCubes;
//     map<uint8, ClientInputs> m_inputs;

    vector<NetCube*> m_bullets;

    void MakeCamera();
    void ProcessMovementInput();

    void UpdateCameraToFollow();

    void MakeSun();
    void SetAmbient( float ambient );


    // Ship
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