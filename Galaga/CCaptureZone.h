#pragma once

#include <glm.hpp>
#include "Component.h"


namespace dae {

	class Scene;

	class CCaptureZone final: public Component {

	public:
		CCaptureZone(GameObjectHandle target, const glm::vec2& relativePos);

		void Start()override;
		void SetActive(bool isActive) override;
		bool CapturedTarget()const;
		void SetActiveCaptureZone(bool setActive); //don't want to activate when gameobject activates


	private:
		dae::GameObjectHandle CreatePlayerDummy(dae::Scene& scene, const glm::vec2& relativePos);
		GameObjectHandle CreateCaptureZone(GameObjectHandle player, dae::Scene& scene, const glm::vec2& relativePos, std::shared_ptr<bool>& outCapturedEnemy);
		
		GameObjectHandle m_CaptureZone;
		GameObjectHandle m_Target;
		GameObjectHandle m_PlayerDummy;

		std::shared_ptr<bool> m_CapturedTarget;
		glm::vec2 m_RelativePos;
		
	};
}