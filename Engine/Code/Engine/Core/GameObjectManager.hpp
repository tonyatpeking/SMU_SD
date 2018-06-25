#pragma once

#include <vector>
#include <map>
#include <string>

class GameObject;

class GameObjectManager
{
    friend class GameObject;
public:
    static GameObjectManager* GetDefault() { return s_default; };
    static void SetDefault( GameObjectManager* manager ) { s_default = manager; };

    GameObjectManager() {};
    virtual ~GameObjectManager() {};

    virtual void Update() {};


    std::vector<GameObject*>& GetObjectsOfType( std::string type );

protected:
    // Only called through GameObject::
    void AddGameObject( GameObject* go );
    void RemoveGameObject( GameObject* go );

    static GameObjectManager* s_default;

    // string is the type of object
    std::map< std::string, std::vector< GameObject* >> m_allGameObjects;
};
