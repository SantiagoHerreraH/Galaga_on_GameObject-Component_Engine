#include "Scene.h"
#include <algorithm>

using namespace dae;

unsigned int Scene::m_idCounter = 0;

Scene::Scene(const std::string& name, const std::function<void(Scene&)>& sceneCreationFunction) : 
	m_SceneCreationFunction(sceneCreationFunction),
	m_Name(name) {}

bool dae::Scene::AddSystem(std::shared_ptr<ISystem> system)
{
	for (size_t i = 0; i < m_Systems.size(); i++)
	{
		if (m_Systems[i] == system)
		{
			return false;
		}
	}

	m_Systems.push_back(system);
	return true;
}

Scene::~Scene() = default;

GameObjectHandle dae::Scene::CreateGameObject()
{
	GameObjectHandle handle{std::make_shared<GameObject>()};

	m_HandlesSet.insert(handle);
	m_HandlesVec.push_back(handle);

	return handle;
}

void dae::Scene::AddGameObjectHandle(const GameObjectHandle& gameObjectHandle)
{
	if (!m_HandlesSet.contains(gameObjectHandle))
	{
		m_HandlesSet.insert(gameObjectHandle);
		m_HandlesVec.push_back(gameObjectHandle);
	}
}

void dae::Scene::RemoveGameObjectHandle(const GameObjectHandle& gameObjectHandle)
{
	if (m_HandlesSet.contains(gameObjectHandle))
	{
		m_HandlesSet.erase(gameObjectHandle);
		std::erase(m_HandlesVec, gameObjectHandle);
	}
}

void dae::Scene::DestroyAllGameObjects()
{
	m_HandlesSet.clear();
	m_HandlesVec.clear();
}

void dae::Scene::Start()
{
	DestroyAllGameObjects();
	m_SceneCreationFunction(*this);

	for (auto& handles : m_HandlesVec)
	{
		handles->Start();
	}
}

void dae::Scene::End()
{
	for (auto& systems : m_Systems)
	{
		systems->Reset();
	}
}

void dae::Scene::FixedUpdate()
{
	for (auto& handles : m_HandlesVec)
	{
		if (handles->IsActive())
		{
			handles->FixedUpdate();
		}

	}
}

void Scene::Update()
{
	for (auto& systems : m_Systems)
	{
		systems->Update();
	}

	for (auto& handles : m_HandlesVec)
	{
		if (handles->IsActive())
		{
			handles->Update();
		}
	}
}

void Scene::Render() const
{
	for (auto& handles : m_HandlesVec)
	{
		if (handles->IsActive())
		{
			handles->Render();
		}
	}
}

