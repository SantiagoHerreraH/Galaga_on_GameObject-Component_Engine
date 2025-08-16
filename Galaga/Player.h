#pragma once

#include <functional>
#include <limits>

#include "Scene.h"
#include "WeaponType.h"
#include "CollisionLayers.h"

namespace dae {

	class CAudio;

	struct PlayerType {
		std::string TextureName{};
		float TextureScale{ 1.f };
		CollisionLayers PlayerCollisionLayer{ CollisionLayers::Player};
		bool HasVerticalMovement{ true };
		std::shared_ptr<WeaponType> WeaponType{};
	};

	class Player 
	{
	public:
		Player() {}
		Player(const glm::vec2& startPos, float zRotation, int playerNum, const PlayerType& PlayerType);

		GameObject& GetGameObject() { return *m_CurrentPlayer; }
		GameObjectHandle GetGameObjectHandle();

		void AddScene(Scene& scene);

		void SubscribeOnPlayerDie(const std::function<void()>& func);
		void SubscribeOnPlayerDespawnFromDamage(const std::function<void()>& func);
		void SubscribeOnPlayerRespawnFromDamage(const std::function<void()>& func);

	private:
		GameObjectHandle m_CurrentPlayer;
		GameObjectHandle m_ShootingPivot;
		GameObjectHandle m_CurrentExplosion;
		std::shared_ptr<CAudio> m_DeathAudio;

	};
}