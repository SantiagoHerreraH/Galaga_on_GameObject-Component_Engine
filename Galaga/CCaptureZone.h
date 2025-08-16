#pragma once

#include <glm.hpp>
#include "CAudio.h"


namespace dae {

	class Scene;

	struct CaptureZoneData {
		GameObjectHandle Target{};
		glm::vec2 RelativePos{};
		AudioData OnActivateAudioData{};
		AudioData OnHitAudioData{};
	};

	class CCaptureZone final: public Component {

	public:
		CCaptureZone(const CaptureZoneData& data);

		void Start()override;
		void SetActive(bool isActive) override;
		bool CapturedTarget()const;
		void SetActiveCaptureZone(bool setActive); //don't want to activate when gameobject activates


	private:
		dae::GameObjectHandle CreatePlayerDummy(dae::Scene& scene, const glm::vec2& relativePos);
		GameObjectHandle CreateCaptureZone(GameObjectHandle player, dae::Scene& scene, const glm::vec2& relativePos, std::shared_ptr<bool>& outCapturedEnemy);
		
		CaptureZoneData m_CaptureZoneData{};

		std::shared_ptr<CAudio> m_OnActivateAudio{};

		GameObjectHandle m_CaptureZone{};
		GameObjectHandle m_PlayerDummy{};

		std::shared_ptr<bool> m_CapturedTarget{};

		
	};
}