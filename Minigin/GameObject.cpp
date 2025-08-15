#include <string>
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Component.h"
#include "Scene.h"
#include "SceneManager.h"

dae::GameObject::GameObject(const std::string& name) : m_Transform(*this), m_Name(name)
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
		m_Components[i].ComponentHandle->SetActive(active);
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

bool dae::GameObject::AlreadyStarted() const
{
	return m_Started;
}

void dae::GameObject::Start()
{

	for (size_t i = 0; i < m_Components.size(); i++)
	{
		m_Components[i].ComponentHandle->Start();
	}

	m_Started = true;
}

void dae::GameObject::Update()
{
	for (size_t i = 0; i < m_Components.size(); i++)
	{
		if (m_Components[i].ComponentHandle->IsActive())
		{
			m_Components[i].ComponentHandle->Update();
		}
	}
}

void dae::GameObject::FixedUpdate()
{
	for (size_t i = 0; i < m_Components.size(); i++)
	{
		if (m_Components[i].ComponentHandle->IsActive())
		{
			m_Components[i].ComponentHandle->FixedUpdate();
		}
	}
}

void dae::GameObject::Render() const
{
	for (size_t i = 0; i < m_Components.size(); i++)
	{
		if (m_Components[i].ComponentHandle->IsActive())
		{
			m_Components[i].ComponentHandle->Render();
		}
	}

	if (m_RenderTransform)
	{
		glm::vec2 pos = TransformConst().GetWorldTransform().Position;
		Renderer::GetInstance().DrawCross(pos, 5, { 0,255,0 });
	}
}


void dae::GameObject::End() {

	for (size_t i = 0; i < m_Components.size(); i++)
	{
		m_Components[i].ComponentHandle->End();
	}

	m_Started = false;
}
