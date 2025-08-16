#pragma once

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Component.h"
#include "GameObject.h"

namespace dae {


	class IState {
	public:
		virtual ~IState() {}
		virtual void Start(GameObject& actor) = 0;
		virtual void Update(GameObject& actor) = 0;
		virtual void End(GameObject& actor) = 0;
	};

	class CStateMachine;

	using StateCondition = std::function<bool(const CStateMachine& self, const GameObject& selfGameObject)>;

	class CStateMachine final : public Component {

	public:
		void Start() override;
		void Update()override;
		void SetActive(bool isActive) override;

		void AddState(const std::shared_ptr<IState>& state, const std::string& stateName);
		bool AddGateCondition(const std::string& stateName, const StateCondition& condition);
		bool AddConnection(const std::string& from, const std::string& to, const StateCondition& condition);
		bool SetState(const std::string& stateName);
		bool SetDefaultState(const std::string& stateName);

		void AddTrigger(const std::string& triggerName);
		void Trigger(const std::string& triggerName);//triggers are active for one tick
		bool HasTrigger(const std::string& triggerName);
		bool IsTriggerActive(const std::string& triggerName)const;

	private:

		struct StateConnection;
		struct StateData {
			std::string StateName;
			std::shared_ptr<IState> State;
			std::vector<dae::StateCondition> GateConditions;
			std::vector<StateConnection> ConnectedStates;

			bool GateConditionsAllow(const CStateMachine& self, const GameObject& selfGameObject) const;
		};
		struct StateConnection {
			std::shared_ptr<StateData> StateData;
			std::vector<dae::StateCondition> ConditionsToTransitionToState;

			bool TransitionAllowed(const CStateMachine& self, const GameObject& selfGameObject)const;
		};
		

		StateConnection& GetStateConnection(StateData& from, const std::string& to);
		std::unordered_map<std::string, std::shared_ptr<StateData>> m_StateName_To_StateData;

		std::shared_ptr<StateData> m_CurrentStateData{nullptr};

		std::string m_DefaultStateName;

		struct TriggerData {
			std::string Name;
			bool State;
		};

		std::vector<TriggerData> m_Triggers;


		TriggerData* GetTrigger(const std::string& triggerName);
		const TriggerData* GetTrigger(const std::string& triggerName)const;
	};

}