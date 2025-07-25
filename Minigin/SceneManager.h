#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Singleton.h"
#include "EventSystem.h"

namespace dae
{
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
		enum class SceneTraversalType {
			Loop,
			Clamp
		};

	public:
		Scene& AddScene(const std::string& name, const std::function<void(dae::Scene&)>& sceneCreationFunction);
		Scene* ChangeCurrentScene(const std::string& name);
		Scene* GetScene(const std::string& name);
		Scene& GetCurrentScene();
		Scene& NextScene(SceneTraversalType sceneTraversalType = SceneTraversalType::Loop);
		Scene& PreviousScene(SceneTraversalType sceneTraversalType = SceneTraversalType::Loop);

		void Start();
		void FixedUpdate();
		void Update();
		void Render()const;

	private:

		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::vector<std::shared_ptr<dae::Scene>> m_Scenes;
		int m_CurrentSceneIndex{0};
	};
}
