#pragma once

#include <functional>
#include <limits>

#include "Scene.h"
#include "Command.h"



namespace dae {

	struct PlayerType {
		std::string Texture;
		std::shared_ptr<ICommand> CommandOnShoot;
	};

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

	};
}