#pragma once
#include "Engine/Core/EngineCommonH.hpp"

class Light;
class Camera;
class GameObject;
class Renderable;
class GameObjectManager;

class RenderSceneGraph
{
public:
    static RenderSceneGraph* GetDefault();
    static void SetCurrentScene( RenderSceneGraph* scene ) { s_default = scene; };

    RenderSceneGraph();
    ~RenderSceneGraph() {};

    vector<GameObject*>& GetGameObjects();
    vector<Renderable*>& GetRenderables();
    vector<GameObject*>& GetLights();

    void AddCamera( Camera* camera ) { m_cameras.push_back( camera ); };
    void RemoveCamera( Camera* camera );
    vector<Camera*>& GetCameras();

    void SetGameObjectManager( GameObjectManager* manager ) { m_manager = manager; };

private:

    vector<Renderable*> m_renderables;
    static RenderSceneGraph* s_default;
    GameObjectManager* m_manager = nullptr;

    vector<Camera*> m_cameras;

};
