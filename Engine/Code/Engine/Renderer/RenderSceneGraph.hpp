#pragma once
#include <vector>

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

    std::vector<GameObject*>& GetGameObjects();
    std::vector<Renderable*>& GetRenderables();
    std::vector<GameObject*>& GetLights();

    void AddCamera( Camera* camera ) { m_cameras.push_back( camera ); };
    void RemoveCamera( Camera* camera );
    std::vector<Camera*>& GetCameras();

    void SetGameObjectManager( GameObjectManager* manager ) { m_manager = manager; };

private:

    std::vector<Renderable*> m_renderables;
    static RenderSceneGraph* s_default;
    GameObjectManager* m_manager = nullptr;

    std::vector<Camera*> m_cameras;

};
