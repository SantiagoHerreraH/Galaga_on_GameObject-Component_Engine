#pragma once

#include <vector>
#include "GameObject.h"
#include <memory>

namespace dae {

	struct NameAssignerSceneData {
		std::vector<GameObjectHandle> GameObjectsToAssignName;
		std::string SceneName;
		std::string SceneNameUponCompletion;
	};



	class NameAssignerScene
	{
	public:
		NameAssignerScene(const NameAssignerSceneData& data);

	private:

		struct NameAssignerSceneInternalData {
			NameAssignerSceneData NameAssignerSceneData;
			int CurrentObjectIdx;
		};

		std::shared_ptr<NameAssignerSceneInternalData> m_NameAssignerSceneInternalData;
	};

}

