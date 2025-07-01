#pragma once

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Component.h"

namespace dae {


	class IState {
	public:
		virtual ~IState() {}
		virtual void Start(GameObject& actor) = 0;
		virtual void Update(GameObject& actor) = 0;
		virtual void End(GameObject& actor) = 0;
	};

	using StateCondition = std::function<bool()>;

	class StateMachine final : public Component {

	public:
		void Start() override;
		void Update()override;

		void AddState(const std::shared_ptr<IState>& state, const std::string& stateName);
		bool AddGateCondition(const std::string& stateName, const StateCondition& condition);
		bool AddConnection(const std::string& from, const std::string& to, const StateCondition& condition);
		bool SetState(const std::string& stateName);

	private:
		
		struct StateConnection;
		struct StateData {
			std::string StateName;
			std::shared_ptr<IState> State;
			std::vector<StateCondition> GateConditions;
			std::vector<StateConnection> ConnectedStates;

			bool GateConditionsAllow() const;
		};
		struct StateConnection {
			std::shared_ptr<StateData> StateData;
			std::vector<StateCondition> ConditionsToTransitionToState;

			bool TransitionAllowed()const;
		};
		

		StateConnection& GetStateConnection(StateData& from, const std::string& to);
		std::unordered_map<std::string, std::shared_ptr<StateData>> m_StateName_To_StateData;

		std::shared_ptr<StateData> m_CurrentStateData{nullptr};
	};

}