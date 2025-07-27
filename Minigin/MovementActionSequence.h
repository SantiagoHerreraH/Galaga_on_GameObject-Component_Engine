#pragma once

#include <glm.hpp>
#include <memory>
#include "TimerSystem.h"
#include "Scene.h"

namespace dae {

	struct MovementData {

		glm::vec2 BeginningDeltaTowardsTarget;
		glm::vec2 BeginningPosition;
	};

	class MovementActionSequence final : public Component 
	{
		
	public:
		MovementActionSequence(const std::string& sequenceName = "NoSequenceName");

		void Start() override;

		const std::string& GetName()const { return m_SequenceName; }
		void SetName(const std::string& sequenceName) { m_SequenceName = sequenceName; }

		bool StartSequence();
		bool RestartSequence();
		void StopSequence();

		bool IsActing()const;

		bool CanStartSequence()const;
		void AddConditionToStartSequence(const std::function<bool()>& condition);//conditions are and 

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
		dae::Scene* m_Scene{ nullptr };
		std::string m_SequenceName{ "NoSequenceName" };

		bool m_StartSequenceOnStart{false};
		bool m_RestartSequenceOnStart{ false };
		bool m_StopSequenceOnStart{ false };

		std::vector<std::function<bool()>> m_ConditionsToStartSequence;

		class MovementAction {

		public:

			void Initialize(MovementActionSequence* movementActionSequence);
			bool WasInitilized()const;

			void RestartAction();
			void StartAction();
			void UpdateAction();
			void EndAction();

			void SetName(const std::string& actionName);
			const std::string& GetName()const;

			TimerKey GetTimerKey();

			float GetDuration();
			void SetDuration(float duration);
			void SetActionDependsOnDuration(bool dependsOnTime, float time = 0);

			bool HasNextTimerKey()const;
			TimerKey GetNextTimerKey();
			void SetNextTimerKey(bool hasNextTimerKey, TimerKey nextTimerKey = 0);

			void AddConditionToStartOrRestartAction(const std::function<bool()>& condition);//conditions are or , if condition false -> end timer
			void AddConditionToStopAction(const std::function<bool()>& condition);//conditions are or 
			
			void SetMovementPathDecidingFunction(std::function<glm::vec2()> functionThatReturnsVectorWithMovementDirectionAndMagnitude);
			void AddActionFunction(std::function<bool(float timeSinceActionStarted, const MovementData&)> movementFuncThatReturnsWhenItIsDone);

			void AddStartSubAction(const std::function<void()>& subAction);
			void AddPostActionUpdateSubAction(const std::function<void()>& subAction);
			void AddEndSubAction(const std::function<void()>& subAction);

		private:

			bool CanStartOrRestartAction()const;
			bool CanStopAction()const;

			MovementActionSequence* m_MovementActionSequence{ nullptr };
			std::string m_ActionName{ "NoActionName" };
			dae::Scene* m_Scene{ nullptr };

			TimerKey m_TimerKey;
			bool m_DependsOnTime;
			float m_Duration;

			bool m_HasNextTimerKey{ false };
			TimerKey m_NextActionTimerKey;

			std::vector<std::function<bool()>> m_ConditionsToStartOrRestart;
			std::vector<std::function<bool()>> m_ConditionsToStopAction;

			MovementData m_MovementData;
			std::function<glm::vec2()> m_MovementPathDecidingFunc;
			std::vector<std::function<bool(float timeSinceActionStarted, const dae::MovementData&)>> m_ActionFunctions;

			Event<> m_StartSubActions;
			Event<> m_UpdateSubActions;
			Event<> m_EndSubActions;
		};

		std::vector<std::shared_ptr<MovementAction>> m_MovementActions;

	};
}



