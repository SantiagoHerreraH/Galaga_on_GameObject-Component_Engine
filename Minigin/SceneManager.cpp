#include "SceneManager.h"
#include "Scene.h"
#include <iostream>

void dae::SceneManager::Start()
{
	CheckChangeScene();
	m_Scenes[m_CurrentSceneIndex]->Start();
}

void dae::SceneManager::FixedUpdate()
{
	m_Scenes[m_CurrentSceneIndex]->FixedUpdate();
}

void dae::SceneManager::Update()
{
	CheckChangeScene();
	m_Scenes[m_CurrentSceneIndex]->Update();
}

void dae::SceneManager::Render()const
{
	m_Scenes[m_CurrentSceneIndex]->Render();
}


void  dae::SceneManager::Clear() {
	m_Scenes.clear();
	m_Scenes.shrink_to_fit();
}

void dae::SceneManager::CheckChangeScene()
{
	if (m_CurrentSceneIndex != m_NextSceneIndex)
	{
		m_Scenes[m_CurrentSceneIndex]->End();
		m_CurrentSceneIndex = m_NextSceneIndex;
		m_Scenes[m_CurrentSceneIndex]->Start();
	}
}

dae::Scene& dae::SceneManager::AddScene(const std::string& name, const std::function<void(Scene&)>& sceneCreationFunction)
{
	const auto& scene = std::shared_ptr<Scene>(new Scene(name, sceneCreationFunction));

	m_Scenes.push_back(scene);
	return *scene;
}

dae::Scene* dae::SceneManager::ChangeCurrentScene(const std::string& name)
{
	for (int i = 0; i < m_Scenes.size(); i++)
	{
		if (m_Scenes[i]->Name() == name)
		{
			if (m_CurrentSceneIndex != i)
			{

				m_NextSceneIndex = i;
			}

			return m_Scenes[i].get();
		}
	}
	return nullptr;
}

dae::Scene* dae::SceneManager::GetScene(const std::string& name)
{
	for (int i = 0; i < m_Scenes.size(); i++)
	{
		if (m_Scenes[i]->Name() == name)
		{

			return m_Scenes[i].get();
		}
	}
	return nullptr;
}

dae::Scene& dae::SceneManager::GetCurrentScene()
{
	return *m_Scenes[m_CurrentSceneIndex].get();
}

dae::Scene& dae::SceneManager::NextScene(SceneTraversalType sceneTraversalType)
{
	switch (sceneTraversalType)
	{
	case SceneTraversalType::Clamp:

		if (m_CurrentSceneIndex == (m_Scenes.size() - 1))
		{
			return *m_Scenes[m_CurrentSceneIndex].get();
		}

		m_NextSceneIndex = m_CurrentSceneIndex + 1;

		return *m_Scenes[m_NextSceneIndex].get();

		break;

	case SceneTraversalType::Loop:


		if (m_CurrentSceneIndex == (m_Scenes.size() - 1))
		{
			m_NextSceneIndex = 0;

		}
		else
		{
			m_NextSceneIndex = m_CurrentSceneIndex + 1;
		}

		return *m_Scenes[m_NextSceneIndex].get();

		break;	
	
	default:

		return *m_Scenes[m_CurrentSceneIndex].get();
		break;
	}
}

dae::Scene& dae::SceneManager::PreviousScene(SceneTraversalType sceneTraversalType)
{
	switch (sceneTraversalType)
	{
	case SceneTraversalType::Clamp:

		if (m_CurrentSceneIndex == 0)
		{
			return *m_Scenes[m_CurrentSceneIndex].get();
		}

		m_NextSceneIndex = m_CurrentSceneIndex - 1;

		return *m_Scenes[m_NextSceneIndex].get();

		break;

	case SceneTraversalType::Loop:


		if (m_CurrentSceneIndex == 0)
		{
			m_NextSceneIndex = int(m_Scenes.size() - 1);

		}
		else
		{
			m_NextSceneIndex = m_CurrentSceneIndex - 1;
		}

		return *m_Scenes[m_NextSceneIndex].get();

		break;

	default:

		return *m_Scenes[m_CurrentSceneIndex].get();
		break;
	}
}
