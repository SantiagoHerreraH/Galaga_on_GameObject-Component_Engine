#pragma once

#include <limits>

#include "Scene.h"
#include "PlayerController.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "Movement.h"
#include "Texture.h"
#include "StatSystem.h"
#include "Text.h"
#include "TimerSystem.h"
#include "EventTriggerCommand.h"

#include "PlayerHealth.h"
#include "GalagaStats.h"
#include "CollisionLayers.h"
#include "PlayerScore.h"
#include "Bullet.h"


namespace dae {

	class GalagaPlayer 
	{
	public:
		GalagaPlayer() {}
		GalagaPlayer(const glm::vec2& startPos, float zRotation);

		GameObjectHandle GetGameObjectHandle();

		void AddScene(Scene& scene);

		void SubscribeOnPlayerDie(const std::function<void()>& func);
		void SubscribeOnPlayerDespawnFromDamage(const std::function<void()>& func);
		void SubscribeOnPlayerRespawnFromDamage(const std::function<void()>& func);

	private:
		GameObjectHandle m_CurrentPlayer;
		GameObjectHandle m_ShootingPivot;
		GameObjectHandle m_CurrentPlayerScore;
		GameObjectHandle m_CurrentPlayerScoreText;

	};
}