#include "Component.h"

bool dae::Component::IsActive() const
{
    return m_IsActive;
}

void dae::Component::SetActive(bool active)
{
    m_IsActive = active;
}

std::type_index dae::Component::GetType()
{
	return std::type_index(typeid(*this));
}

void dae::Component::SetOwner(GameObject& owner)
{
    m_Owner = &owner;
}
