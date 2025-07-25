#include <string>
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Component.h"
#include "Scene.h"
#include "SceneManager.h"

dae::GameObject::GameObject() : m_Transform(*this)
{
}

dae::GameObject::~GameObject() = default;

dae::Transform& dae::GameObject::Transform()
{
	return m_Transform;
}

const dae::Transform& dae::GameObject::TransformConst() const
{
	return m_Transform;
}

bool dae::GameObject::IsActive() const
{
	return m_IsActive;
}

void dae::GameObject::SetActive(bool active, bool affectChildren)
{
	m_IsActive = active;

	for (size_t i = 0; i < m_Components.size(); i++)
	{
		m_Components[i]->SetActive(active);
	}

	if (affectChildren)
	{
		std::vector<GameObject*> children = m_Transform.GetChildren();

		for (size_t i = 0; i < children.size(); i++)
		{
			children[i]->SetActive(active, true);
		}
	}
}

void dae::GameObject::Start()
{

	for (size_t i = 0; i < m_Components.size(); i++)
	{
		if (m_Components[i]->IsActive())
		{
			m_Components[i]->Start();
		}
	}
}

void dae::GameObject::Update()
{
	for (size_t i = 0; i < m_Components.size(); i++)
	{
		if (m_Components[i]->IsActive())
		{
			m_Components[i]->Update();
		}
	}
}

void dae::GameObject::FixedUpdate()
{
	for (size_t i = 0; i < m_Components.size(); i++)
	{
		if (m_Components[i]->IsActive())
		{
			m_Components[i]->FixedUpdate();
		}
	}
}

void dae::GameObject::Render() const
{
	for (size_t i = 0; i < m_Components.size(); i++)
	{
		if (m_Components[i]->IsActive())
		{
			m_Components[i]->Render();
		}
	}
}
