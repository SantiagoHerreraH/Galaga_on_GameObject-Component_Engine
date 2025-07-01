#include "StateSystem.h"

dae::StateController::StateController(const Pillar::UEntityHandle& entityHandle, Pillar::URegistry& registry) :
	m_EntityHandle(entityHandle),
	m_Registry(&registry){}

void dae::StateController::AssignState(const std::string& stateName)
{
	if (m_CurrentStateName != stateName)
	{
		m_CurrentStateName = stateName;

		for (size_t i = 0; i < m_States.size(); i++)
		{
			m_States[i]->AssignState(stateName);
		}
	}
}

void dae::StateController::RemoveState(const std::string& stateName)
{
	for (size_t i = 0; i < m_States.size(); i++)
	{
		m_States[i]->RemoveState(stateName);
	}
}
