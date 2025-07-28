#pragma once
#include <cassert>
#include <memory>
#include <unordered_map>
#include "ISystem.h"
#include "Scene.h"

namespace dae {


	template<typename Child>
	class SceneSingleton : public ISystem
	{
		dae::Scene* m_Scene{nullptr};
		static std::unordered_map<dae::Scene*, std::shared_ptr<Child>> m_ScenePtr_To_Child;

		void SetScene(dae::Scene* scene) { m_Scene = scene; } //to avoid having to add the scene ptr to the constructor in child classes
		
	protected:
		SceneSingleton() = default;
	public:
		
		static Child& GetCurrent() {
			return GetFromScene(&SceneManager::GetInstance().GetCurrentScene());
		}

		static Child& GetFromScene(dae::Scene* scene) 
		{ 

			assert(scene != nullptr);

			if (!m_ScenePtr_To_Child.contains(scene))
			{
				auto value = std::make_shared<Child>(); 

				auto* casted = dynamic_cast<SceneSingleton<Child>*>(value.get());
				assert(casted != nullptr && "dynamic_cast failed: Incorrect inheritance hierarchy");

				casted->SetScene(scene);
				m_ScenePtr_To_Child.emplace(scene, value);
				scene->AddSystem(value); 
			}

			return *m_ScenePtr_To_Child[scene];
		}

		Scene& GetScene() { return *m_Scene; }
		Scene* GetScenePtr() { return m_Scene; }

		virtual ~SceneSingleton() noexcept = default;
		SceneSingleton(const SceneSingleton& other) = delete;
		SceneSingleton(SceneSingleton&& other) = delete;
		SceneSingleton& operator=(const SceneSingleton& other) = delete;
		SceneSingleton& operator=(SceneSingleton&& other) = delete;

	};

	template<typename Child>
	std::unordered_map<dae::Scene*, std::shared_ptr<Child>> SceneSingleton<Child>::m_ScenePtr_To_Child;

}


