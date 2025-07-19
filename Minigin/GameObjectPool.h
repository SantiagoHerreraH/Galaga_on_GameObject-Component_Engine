#pragma once

#include <functional>
#include <vector>
#include <string>
#include <tuple>
#include <utility>
#include <glm.hpp>
#include "GameObject.h"
#include "TimerSystem.h"


namespace dae {


	template<typename ... Args>
	class GameObjectPool
	{
	public:
		GameObjectPool(Scene& scene, const std::function<GameObjectHandle(const Args&...args)>& gameObjectCreationFunction, const std::tuple<Args...>& argsTuple, size_t numberOfGameObjects, float lifetime = -1);

		bool GameObjectsHaveLifetime()const;
		float GetGameObjectsLifetime()const;//negative, means it doesn't have lifetime (infinite until despawned)
		void SetGameObjectsLifetime(float lifeTime);//negative, means it doesn't have lifetime (infinite until despawned)

		void SpawnNext(const glm::vec2 where);
		void Despawn(const GameObjectHandle& who);

	private:
		std::vector<GameObjectHandle> m_GameObjects;
		std::vector<TimerKey> m_GameObjectLifeTimes;
		size_t m_LastSpawned{ 0 };
		float m_Lifetime{ -1 };
		TimerSystem* m_TimerSystem{nullptr};
	};

	template<typename ...Args>
	inline GameObjectPool<Args...>::GameObjectPool(Scene& scene, const std::function<GameObjectHandle(const Args&...args)>& gameObjectCreationFunction, const std::tuple<Args...>& argsTuple, size_t numberOfGameObjects, float lifetime) :
		m_Lifetime(lifetime),
		m_TimerSystem(&TimerSystem::GetFromScene(&scene))
	{
		m_GameObjects.reserve(numberOfGameObjects);
		
		GameObjectHandle gameObj{nullptr};

		for (size_t i = 0; i < numberOfGameObjects; i++)
		{
			gameObj = std::apply(gameObjectCreationFunction, argsTuple);

			Timer lifetimeTimer{ m_Lifetime , m_Lifetime < 0, "GameObjectLifeTime " + std::to_string(i)};
			lifetimeTimer.GetOnEndEvent().Subscribe([gameObj]() {

				gameObj->SetActive(false);

				});

			m_GameObjectLifeTimes.push_back(m_TimerSystem->AddTimer(lifetimeTimer));
			m_GameObjects.push_back(gameObj);
		}


	}

	template<typename ...Args>
	inline bool GameObjectPool<Args...>::GameObjectsHaveLifetime() const
	{
		return m_Lifetime >= 0;
	}

	
	template<typename ...Args>
	inline float GameObjectPool<Args...>::GetGameObjectsLifetime()const
	{
		return m_Lifetime;
	}

	template<typename ...Args>
	inline void GameObjectPool<Args...>::SetGameObjectsLifetime(float lifeTime)
	{
		m_Lifetime = lifeTime;

		for (size_t i = 0; i < m_GameObjectLifeTimes.size(); i++)
		{
			m_TimerSystem->SetTimerDuration(m_GameObjectLifeTimes[i], m_Lifetime, false);
		}
		
	}

	

	template<typename ...Args>
	inline void GameObjectPool<Args...>::SpawnNext(const glm::vec2 where)
	{
		m_GameObjects[m_LastSpawned]->Transform().SetLocalPositionX(where.x);
		m_GameObjects[m_LastSpawned]->Transform().SetLocalPositionY(where.y);
		m_GameObjects[m_LastSpawned]->SetActive(true);

		if (m_Lifetime > 0)
		{
			m_TimerSystem->RestartTimer(m_GameObjectLifeTimes[m_LastSpawned]);
		}

		m_LastSpawned = (m_LastSpawned + 1) % m_GameObjects.size();
	}

	template<typename ...Args>
	inline void GameObjectPool<Args...>::Despawn(const GameObjectHandle& who)
	{
		who->SetActive(false);
	}

}

