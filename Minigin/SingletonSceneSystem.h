#pragma once
#include <cassert>
#include <memory>
#include <unordered_map>
#include "ISystem.h"
#include "Scene.h"

namespace dae {


	template<typename Child>
	class SingletonSceneSystem : public ISystem
	{
		std::shared_ptr<Scene> m_Scene;
		static std::unordered_map<std::shared_ptr<Scene>, std::shared_ptr<Child>> m_ScenePtr_To_Child;

		void SetScene(std::shared_ptr<Scene> scene) { m_Scene = scene; } //to avoid having to add the scene ptr to the constructor in child classes
		SingletonSceneSystem() = default;
	public:

		static Child& GetFromScene(const std::shared_ptr<Scene>& scene) {

			assert(scene != nullptr);

			if (!m_ScenePtr_To_Child.contains(scene))
			{
				auto value = std::make_shared<Child>(); 
				auto* casted = dynamic_cast<SingletonSceneSystem<Child>*>(value.get());
				assert(casted != nullptr && "dynamic_cast failed: Incorrect inheritance hierarchy");
				casted->SetScene(scene);
				m_ScenePtr_To_Child.emplace(scene, value);
				scene->AddSystem(value); 
			}

			return *m_ScenePtr_To_Child[scene];
		}

		Scene& GetScene() { return *m_Scene; }
		std::shared_ptr<Scene> GetSceneHandle() { return m_Scene; }

		virtual ~SingletonSceneSystem() noexcept = default;
		SingletonSceneSystem(const SingletonSceneSystem& other) = delete;
		SingletonSceneSystem(SingletonSceneSystem&& other) = delete;
		SingletonSceneSystem& operator=(const SingletonSceneSystem& other) = delete;
		SingletonSceneSystem& operator=(SingletonSceneSystem&& other) = delete;

	};
}


