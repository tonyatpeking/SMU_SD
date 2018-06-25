#include "Engine/Core/GameObjectManager.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Core/ContainerUtils.hpp"

GameObjectManager* GameObjectManager::s_default = nullptr;

std::vector<GameObject*>& GameObjectManager::GetObjectsOfType( std::string type )
{
    return m_allGameObjects[type];
}

void GameObjectManager::AddGameObject( GameObject* go )
{
    std::string goType = go->GetType();
    std::vector<GameObject*>& gos = GetObjectsOfType( goType );
    gos.push_back( go );
}

void GameObjectManager::RemoveGameObject( GameObject* go )
{
    std::string goType = go->GetType();
    std::vector<GameObject*>& gos = GetObjectsOfType( goType );
    ContainerUtils::EraseOneValue( gos, go );
}

