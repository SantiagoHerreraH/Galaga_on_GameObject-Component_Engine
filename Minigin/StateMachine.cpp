#include "StateMachine.h"

namespace dae {
	void CStateMachine::Start()
	{
		if (!m_CurrentStateData)
		{
			m_CurrentStateData = m_StateName_To_StateData.begin()->second;
		}

		m_CurrentStateData->State->Start(Owner());
	}
	void CStateMachine::Update()
	{
		m_CurrentStateData->State->Update(Owner());

		for (size_t i = 0; i < m_CurrentStateData->ConnectedStates.size(); i++)
		{
			if (m_CurrentStateData->ConnectedStates[i].TransitionAllowed(*this, OwnerConst()))
			{
				m_CurrentStateData->State->End(Owner());
				m_CurrentStateData = m_CurrentStateData->ConnectedStates[i].StateData;
				m_CurrentStateData->State->Start(Owner());
				break;
			}
		}

		for (size_t i = 0; i < m_Triggers.size(); i++)
		{
			m_Triggers[i].State = false;
		}
	}

	void CStateMachine::SetActive(bool isActive)
	{
		Component::SetActive(isActive);

		if (isActive)
		{
			SetState(m_DefaultStateName);
		}
	}

	void dae::CStateMachine::AddState(const std::shared_ptr<IState>& state, const std::string& stateName)
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

	bool dae::CStateMachine::AddGateCondition(const std::string& stateName, const StateCondition& condition)
	{
		if (m_StateName_To_StateData.contains(stateName))
		{
			m_StateName_To_StateData[stateName]->GateConditions.push_back(condition);

			return true;
		}

		return false;
	}

	bool dae::CStateMachine::AddConnection(const std::string& from, const std::string& to, const StateCondition& condition)
	{
		if (m_StateName_To_StateData.contains(from) && m_StateName_To_StateData.contains(to))
		{
			StateData& fromData = *m_StateName_To_StateData[from].get();
			StateConnection& stateConnection = GetStateConnection(fromData, to);
			stateConnection.ConditionsToTransitionToState.push_back(condition);

			return true;
		}

		return false;
	}

	bool dae::CStateMachine::SetState(const std::string& stateName)
	{
		if (m_StateName_To_StateData.contains(stateName))
		{
			if (m_CurrentStateData)
			{
				m_CurrentStateData->State->End(Owner());
			}

			m_CurrentStateData = m_StateName_To_StateData[stateName];

			m_CurrentStateData->State->Start(Owner());

			return true;
		}

		return false;
	}

	bool dae::CStateMachine::SetDefaultState(const std::string& stateName) 
	{
		if (m_StateName_To_StateData.contains(stateName))
		{
			m_DefaultStateName = stateName;

			return true;
		}

		return false;
	}


	CStateMachine::StateConnection& CStateMachine::GetStateConnection(StateData& from, const std::string& to)
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

	dae::CStateMachine::TriggerData* CStateMachine::GetTrigger(const std::string& triggerName)
	{
		for (size_t i = 0; i < m_Triggers.size(); i++)
		{
			if (m_Triggers[i].Name == triggerName)
			{

				return &m_Triggers[i];
			}
		}
		return nullptr;
	}

	const dae::CStateMachine::TriggerData* CStateMachine::GetTrigger(const std::string& triggerName) const
	{
		for (size_t i = 0; i < m_Triggers.size(); i++)
		{
			if (m_Triggers[i].Name == triggerName)
			{

				return &m_Triggers[i];
			}
		}
		return nullptr;
	}

	bool dae::CStateMachine::StateConnection::TransitionAllowed(const CStateMachine& self, const GameObject& selfGameObject) const
	{
		if (!StateData->GateConditionsAllow(self, selfGameObject))
		{
			return false;
		}
		for (size_t i = 0; i < ConditionsToTransitionToState.size(); i++)
		{
			if (!ConditionsToTransitionToState[i](self, selfGameObject))
			{
				return false;
			}
		}

		return true;
	}

	bool dae::CStateMachine::StateData::GateConditionsAllow(const CStateMachine& self, const GameObject& selfGameObject) const
	{
		for (size_t i = 0; i < GateConditions.size(); i++)
		{
			if (!GateConditions[i](self, selfGameObject))
			{
				return false;
			}
		}

		return true;
	}


	void dae::CStateMachine::AddTrigger(const std::string& triggerName) {

		if (!HasTrigger(triggerName))
		{
			m_Triggers.push_back(TriggerData{ triggerName, false });
		}
	}
	void dae::CStateMachine::Trigger(const std::string& triggerName) {

		TriggerData* data = GetTrigger(triggerName);
		if (data)
		{
			data->State = true;
		}
	}
	bool CStateMachine::HasTrigger(const std::string& triggerName)
	{
		for (size_t i = 0; i < m_Triggers.size(); i++)
		{
			if (m_Triggers[i].Name == triggerName)
			{
				return true;
			}
		}
		return false;
	}
	bool dae::CStateMachine::IsTriggerActive(const std::string& triggerName)const {
		
		const TriggerData* data = GetTrigger(triggerName);

		if (data)
		{
			return data->State;
		}

		return false;
	}
}

