#include "StateMachine.h"

namespace dae {
	void StateMachine::Start()
	{
		if (!m_CurrentStateData)
		{
			m_CurrentStateData = m_StateName_To_StateData.begin()->second;
		}

		m_CurrentStateData->State->Start(Owner());
	}
	void StateMachine::Update()
	{
		m_CurrentStateData->State->Update(Owner());

		for (size_t i = 0; i < m_CurrentStateData->ConnectedStates.size(); i++)
		{
			if (m_CurrentStateData->ConnectedStates[i].TransitionAllowed())
			{
				m_CurrentStateData->State->End(Owner());
				m_CurrentStateData = m_CurrentStateData->ConnectedStates[i].StateData;
				m_CurrentStateData->State->Start(Owner());
				break;
			}
		}
	}

	void dae::StateMachine::AddState(const std::shared_ptr<IState>& state, const std::string& stateName)
	{
		if (m_StateName_To_StateData.contains(stateName))
		{
			m_StateName_To_StateData[stateName]->State = state;
		}
		else
		{
			auto statData = std::make_shared<StateData>();
			statData->State = state;

			m_StateName_To_StateData.emplace(stateName, statData);
		}
	}

	bool dae::StateMachine::AddGateCondition(const std::string& stateName, const StateCondition& condition)
	{
		if (m_StateName_To_StateData.contains(stateName))
		{
			m_StateName_To_StateData[stateName]->GateConditions.push_back(condition);

			return true;
		}

		return false;
	}

	bool dae::StateMachine::AddConnection(const std::string& from, const std::string& to, const StateCondition& condition)
	{
		if (m_StateName_To_StateData.contains(from) && m_StateName_To_StateData.contains(to))
		{
			StateData& fromData = *m_StateName_To_StateData[from].get();
			StateData& toData = *m_StateName_To_StateData[to].get();
			StateConnection& stateConnection = GetStateConnection(fromData, to);
			stateConnection.ConditionsToTransitionToState.push_back(condition);

			return true;
		}

		return false;
	}

	bool dae::StateMachine::SetState(const std::string& stateName)
	{
		if (m_StateName_To_StateData.contains(stateName))
		{
			m_CurrentStateData = m_StateName_To_StateData[stateName];

			return true;
		}

		return false;
	}

	StateMachine::StateConnection& StateMachine::GetStateConnection(StateData& from, const std::string& to)
	{
		for (size_t i = 0; i < from.ConnectedStates.size(); i++)
		{
			if (from.ConnectedStates[i].StateData->StateName == to)
			{
				return from.ConnectedStates[i];
			}
		}
		StateConnection newConnection{};
		newConnection.StateData = m_StateName_To_StateData[to];

		from.ConnectedStates.push_back(newConnection);

		return from.ConnectedStates.back();
	}

	bool dae::StateMachine::StateConnection::TransitionAllowed() const
	{
		if (!StateData->GateConditionsAllow())
		{
			return false;
		}
		for (size_t i = 0; i < ConditionsToTransitionToState.size(); i++)
		{
			if (!ConditionsToTransitionToState[i]())
			{
				return false;
			}
		}

		return true;
	}

	bool dae::StateMachine::StateData::GateConditionsAllow() const
	{
		for (size_t i = 0; i < GateConditions.size(); i++)
		{
			if (!GateConditions[i]())
			{
				return false;
			}
		}

		return true;
	}


}

