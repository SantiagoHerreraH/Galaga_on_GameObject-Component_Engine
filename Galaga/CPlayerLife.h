#pragma once

#include <glm.hpp>
#include "Component.h"
#include "EventSystem.h"
#include "CGameStatController.h"
#include "InputManager.h"

namespace dae {



	class CPlayerController;

	class CPlayerLife final: public Component
	{
	public:
		CPlayerLife(int maxHealth, const glm::vec2& respawnPos, float secondsToRespawn, int playerNum) : 
			m_MaxHealth(maxHealth),
			m_RespawnPos(respawnPos),
			m_PlayerNum(playerNum),
			m_SecondsToRespawn(secondsToRespawn){}
		virtual void Start()override;
		virtual void End() override;

		void SubscribeOnPlayerDie(const std::function<void()>& func);
		void SubscribeOnPlayerDespawnFromDamage(const std::function<void()>& func);
		void SubscribeOnPlayerRespawnFromDamage(const std::function<void()>& func);

	private:
		CGameStatController* GetStatController();
		CPlayerController* GetPlayerController();
		std::vector<dae::GameObjectHandle> CreatePlayerHealthIndicator(dae::PlayerId playerId, int playerHealth, const glm::vec2& startPos, float symbolOffset, float textOffset);
		
		float m_SecondsToRespawn;
		int m_PlayerNum;
		int m_MaxHealth;
		glm::vec2 m_RespawnPos;
		Event<> m_OnDie;
		Event<> m_OnRespawnFromDamage;
		Event<> m_OnDespawnFromDamage;
	};

}

