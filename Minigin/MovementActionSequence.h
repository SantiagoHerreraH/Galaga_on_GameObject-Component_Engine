#pragma once

#include <glm.hpp>
#include <memory>
#include "Time.h"
#include "GameObject.h"

namespace dae {

	struct MovementData {

		glm::vec2 BeginningDeltaTowardsTarget;
		glm::vec2 BeginningPosition;
	};

	struct MovementAction {


		TimerKey TimerKey;

		std::shared_ptr<MovementData> MovementData;

		bool LoopsInSpace;
		glm::vec2 topLeftBounds;
		glm::vec2 bottomRightBounds;

	};

	class MovementActionSequence
	{
		
	public:
		MovementActionSequence(const GameObject& self, const std::string& sequenceName = "NoSequenceName");

		const GameObject& GetSelf()const { return m_Self; }
		const std::string& GetName()const { return m_SequenceName; }
		void SetName(const std::string& sequenceName) { m_SequenceName = sequenceName; }

		bool StartSequence();
		bool RestartSequence();
		void StopSequence();

		bool CanStartSequence()const;
		void AddConditionToStartSequence(const std::function<bool()>& condition);//conditions are and 

		TimerKey GetFirstActionTimerKey()const;
		std::vector<TimerKey> GetActionTimerKeys()const;

		void AddAction(const std::string& actionName = "noActionName");

		void SetActionDependsOnDuration(bool dependsOnTime, float time = 0);
		void SetActionDuration(float time);

		void AddConditionToStartOrRestartAction(const std::function<bool()>& condition);//conditions are or , if condition false -> end timer
		void AddConditionToStopAction(const std::function<bool()>& condition);//conditions are or 

		void SetMovementPathDecidingFunction(std::function<glm::vec2()> functionThatReturnsVectorWithMovementDirectionAndMagnitude);
		void AddActionFunction(std::function<bool(float timeSinceActionStarted, const MovementData&)> movementFuncThatReturnsWhenItIsDone);

		void AddStartSubAction(const std::function<void()>& subAction);
		void AddUpdateSubAction(const std::function<void()>& subAction);
		void AddEndSubAction(const std::function<void()>& subAction);

	private:

		GameObject m_Self;
		std::string m_SequenceName;

		std::vector<std::function<bool()>> m_ConditionsToStartSequence;
		std::vector<MovementAction> m_MovementActions;

	};
}



