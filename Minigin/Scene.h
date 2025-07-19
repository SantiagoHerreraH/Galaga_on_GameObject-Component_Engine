#pragma once
#include <unordered_set>
#include <any>
#include <vector>
#include <string>
#include <typeindex>
#include "SceneManager.h"
#include "ISystem.h"
#include "GameObject.h"
#include "Component.h"

namespace dae
{

	class Scene final
	{
		friend Scene& SceneManager::AddScene(const std::string& name, const std::function<void(Scene&)>& sceneCreationFunction);
	public:
		
		/*
			ORDER OF ADDITION IS ORDER OF EXECUTION
		*/
		GameObjectHandle CreateGameObject();
		void AddGameObjectHandle(const GameObjectHandle& gameObjectHandle);
		void RemoveGameObjectHandle(const GameObjectHandle& gameObjectHandle);
		void DestroyAllGameObjects();

		bool AddSystem(std::shared_ptr<ISystem> system);

		void Start();
		void FixedUpdate();
		void Update();
		void Render() const;
		void End();

		template<DerivedFromComponent ComponentType>
		std::vector<std::shared_ptr<ComponentType>> GetAllComponentsOfType();

		const std::string& Name() const { return m_Name; }

		~Scene();
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private: 
		explicit Scene(const std::string& name, const std::function<void(Scene&)>& sceneCreationFunction);

		std::function<void(Scene&)> m_SceneCreationFunction;

		std::string m_Name;

		std::unordered_set<GameObjectHandle> m_HandlesSet{};
		std::vector<GameObjectHandle> m_HandlesVec{}; 
		std::vector<std::shared_ptr<ISystem>> m_Systems{};

		static unsigned int m_idCounter; 
	};


	using SceneHandle = std::shared_ptr<Scene>;

	template<DerivedFromComponent ComponentType>
	inline std::vector<std::shared_ptr<ComponentType>> Scene::GetAllComponentsOfType()
	{
		std::vector<std::shared_ptr<ComponentType>> comps{};

		for (size_t gameObjIdx = 0; gameObjIdx < m_HandlesVec.size(); gameObjIdx++)
		{
			auto gameObjComps{ m_HandlesVec[gameObjIdx]->GetComponentHandles<ComponentType>() };

			for (size_t compIdx = 0; compIdx < gameObjComps.size(); compIdx++)
			{
				comps.push_back(gameObjComps[compIdx]);
			}
				
		}

		return comps;
	}

}
