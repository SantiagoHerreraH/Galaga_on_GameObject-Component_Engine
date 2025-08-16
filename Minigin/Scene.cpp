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

Scene::~Scene() {
	Reset();
}

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


void dae::Scene::Reset() 
{

	for (size_t i = 0; i < m_Systems.size(); i++)
	{
		m_Systems[i]->Reset();
	}

	m_Systems.clear();
	m_HandlesSet.clear();
	m_HandlesVec.clear();
}

void dae::Scene::Start()
{
	DestroyAllGameObjects();
	m_SceneCreationFunction(*this);

	for (size_t i = 0; i < m_HandlesVec.size(); i++)
	{
		m_HandlesVec[i]->Start();
	}
}

void dae::Scene::End()
{
	for (size_t i = 0; i < m_Systems.size(); i++)
	{
		m_Systems[i]->Reset();
	}

	for (size_t i = 0; i < m_HandlesVec.size(); i++)
	{
		m_HandlesVec[i]->End();
	}
}

void dae::Scene::FixedUpdate()
{
	for (size_t i = 0; i < m_HandlesVec.size(); i++)
	{
		if (m_HandlesVec[i]->IsActive())
		{
			m_HandlesVec[i]->FixedUpdate();
		}
	}
}

void Scene::Update()
{
	for (size_t i = 0; i < m_Systems.size(); i++)
	{
		m_Systems[i]->Update();
	}

	for (size_t i = 0; i < m_HandlesVec.size(); i++)
	{
		if (m_HandlesVec[i]->IsActive())
		{
			m_HandlesVec[i]->Update();
		}
	}
}

void Scene::Render() const
{
	for (size_t i = 0; i < m_HandlesVec.size(); i++)
	{
		if (m_HandlesVec[i]->IsActive())
		{
			m_HandlesVec[i]->Render();
		}
	}
}

