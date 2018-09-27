#pragma once

#include "Engine/Core/EngineCommonH.hpp"

class GameObject;

typedef vector<GameObject*> GameObjects;

class GameObjectManager
{
    friend class GameObject;
public:
    static GameObjectManager* GetDefault();
    static void SetDefault( GameObjectManager* manager ) { s_default = manager; };

    GameObjectManager() {};
    virtual ~GameObjectManager() {};

    virtual void Update();
    virtual void DeleteDeadGameObjects();

    GameObjects& GetObjectsOfType( string type );
    GameObjects& GetObejctsFlat();

protected:
    // Only called through GameObject::
    void AddGameObject( GameObject* go );
    void RemoveGameObject( GameObject* go );

    static GameObjectManager* s_default;

    // string is the type of object
    map< string, GameObjects> m_allGameObjects;
    GameObjects m_allGameObjectsFlat;
};
