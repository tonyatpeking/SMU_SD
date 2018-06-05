#pragma once
#include <vector>

class Light;
class Camera;
class GameObject;
class Renderable;

class RenderSceneGraph
{
public:
    static RenderSceneGraph* GetCurrentScene() { return s_currentScene; };
    static void SetCurrentScene( RenderSceneGraph* scene ) { s_currentScene = scene; };

    RenderSceneGraph() {};
    ~RenderSceneGraph() {};

    void AddGameObject( GameObject* gameObject );
    void AddLight( Light* light );
    void AddCamera( Camera* camera );

    void RemoveGameObject( GameObject* gameObject );
    void RemoveLight( Light* light );
    void RemoveCamera( Camera* camera );

    std::vector<GameObject*>& GetGameObjects() { return m_gameObjects; };
    std::vector<Renderable*>& GetRenderables();
    std::vector<Light*>& GetLights() { return m_lights; };
    std::vector<Camera*>& GetCameras() { return m_cameras; };


private:

    static RenderSceneGraph* s_currentScene;

    std::vector<GameObject*> m_gameObjects;
    std::vector<Renderable*> m_renderables;

    std::vector<Light*> m_lights;
    std::vector<Camera*> m_cameras;


};
