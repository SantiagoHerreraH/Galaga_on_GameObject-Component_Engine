#pragma once
#include "Scene.h"
#include <glm.hpp>

namespace dae {

	struct EnemyInstanceData {
		class CEnemyFormation* EnemyManager;
		dae::Scene* Scene;
		GameObjectHandle Grid;
		glm::vec2 RelativePos;
		GameObjectHandle Player;
		GameObjectHandle Enemy{nullptr}; //if nullptr, create, else use
		bool MoveTowardsLeft;
	};
}