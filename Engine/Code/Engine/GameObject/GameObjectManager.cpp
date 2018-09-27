#include "Engine/GameObject/GameObjectManager.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Core/ContainerUtils.hpp"

GameObjectManager* GameObjectManager::s_default = nullptr;

GameObjectManager* GameObjectManager::GetDefault()
{
    if( !s_default )
        s_default = new GameObjectManager();

    return s_default;
}

void GameObjectManager::Update()
{
    for (int i = 0; i < m_allGameObjectsFlat.size() ; ++i)
    {
        m_allGameObjectsFlat[i]->Update();
    }
}

void GameObjectManager::DeleteDeadGameObjects()
{
    int count = (int)m_allGameObjectsFlat.size();
    // loop removes elements from vectors!
    for (int i = count - 1; i >= 0 ; --i)
    {
        GameObject* go = m_allGameObjectsFlat[i];
        if( go->ShouldDie() )
        {
            delete go;
        }
    }
}

vector<GameObject*>& GameObjectManager::GetObjectsOfType( string type )
{
    return m_allGameObjects[type];
}

vector<GameObject*>& GameObjectManager::GetObejctsFlat()
{
    return m_allGameObjectsFlat;
}

void GameObjectManager::AddGameObject( GameObject* go )
{
    string goType = go->GetType();
    vector<GameObject*>& gos = GetObjectsOfType( goType );

    gos.push_back( go );
    m_allGameObjectsFlat.push_back( go );
}

void GameObjectManager::RemoveGameObject( GameObject* go )
{
    string goType = go->GetType();
    vector<GameObject*>& gos = GetObjectsOfType( goType );
    ContainerUtils::EraseOneValue( gos, go );
    ContainerUtils::EraseOneValue( m_allGameObjectsFlat, go );
}

