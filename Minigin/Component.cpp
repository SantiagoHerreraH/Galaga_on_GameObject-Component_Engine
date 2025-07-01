#include "Component.h"

bool dae::Component::IsActive() const
{
    return m_IsActive;
}

void dae::Component::SetActive(bool active)
{
    m_IsActive = active;
}

const std::type_info& dae::Component::GetType() const
{
	return typeid(*this);
}

void dae::Component::SetOwner(GameObject& owner)
{
    m_Owner = std::make_shared<GameObject>(&owner);
}
