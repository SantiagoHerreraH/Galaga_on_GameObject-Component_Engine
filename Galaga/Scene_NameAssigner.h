#pragma once

#include <vector>
#include "GameObject.h"
#include <memory>

namespace dae {

	struct SceneData_NameAssigner {
		std::vector<GameObjectHandle> GameObjectsToAssignName{};
		std::string SceneName{};
		std::string SceneNameUponCompletion{};
	};



	class Scene_NameAssigner
	{
	public:
		Scene_NameAssigner(const SceneData_NameAssigner& data);

	private:

		struct NameAssignerSceneInternalData {
			SceneData_NameAssigner NameAssignerSceneData{};
			int CurrentObjectIdx{};
		};

		std::shared_ptr<NameAssignerSceneInternalData> m_NameAssignerSceneInternalData{};
	};

}

