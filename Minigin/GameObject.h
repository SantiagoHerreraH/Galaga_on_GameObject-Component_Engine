#pragma once
#include <memory>
#include "Transform.h"
#include "Component.h"

namespace dae
{
	template<typename T>
	concept DerivedFromComponent = std::derived_from<T, Component>;

	class Scene;

	class GameObject final
	{

	public:
		GameObject();
		virtual ~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		void Start();
		void Update();
		void FixedUpdate();
		void Render() const;

		bool IsActive()const;
		void SetActive(bool active, bool affectChildren = true);

		const dae::Transform& TransformConst()const;
		dae::Transform& Transform();

		template <DerivedFromComponent ComponentType>
		std::shared_ptr<ComponentType> AddComponent(const ComponentType& component);

		template <DerivedFromComponent ComponentType>
		bool HasComponent()const;
		template <DerivedFromComponent ComponentType>
		ComponentType* GetComponent();
		template <DerivedFromComponent ComponentType>
		const ComponentType* const GetComponentConst()const;
		template <DerivedFromComponent ComponentType>
		std::shared_ptr<ComponentType> GetComponentHandle();
		template <DerivedFromComponent ComponentType>
		std::vector<std::shared_ptr<ComponentType>> GetComponentHandles();

		template< DerivedFromComponent ComponentType>
		bool SetComponent(const ComponentType& component)const;

	private:

		dae::Transform m_Transform;
		bool m_IsActive{ true };
		std::vector<ComponentHandle> m_Components;

	};

	using GameObjectHandle = std::shared_ptr<GameObject>;
	

	template<DerivedFromComponent ComponentType>
	inline std::shared_ptr<ComponentType> GameObject::AddComponent(const ComponentType& component)
	{
		m_Components.push_back(std::make_shared<Component>(component));
		m_Components.back()->SetOwner(*this);

		return std::dynamic_pointer_cast<ComponentType>(m_Components.back());
	}

	template<DerivedFromComponent ComponentType>
	inline bool GameObject::HasComponent() const
	{
		std::type_index typeIndex{ typeid(ComponentType) };

		for (size_t i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i]->GetType() == typeIndex)
			{
				return true;
			}
		}

		return false;
	}

	template<DerivedFromComponent ComponentType>
	inline ComponentType* GameObject::GetComponent()
	{
		std::type_index typeIndex{ typeid(ComponentType) };

		for (size_t i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i]->GetType() == typeIndex)
			{
				return m_Components[i].get();
			}
		}

		return nullptr;
	}

	template<DerivedFromComponent ComponentType>
	inline const ComponentType* const GameObject::GetComponentConst() const
	{
		std::type_index typeInfo{ typeid(ComponentType) };

		for (size_t i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i]->GetType() == typeInfo)
			{
				return m_Components[i].get();
			}
		}

		return nullptr;
	}
	template<DerivedFromComponent ComponentType>
	inline std::shared_ptr<ComponentType> GameObject::GetComponentHandle() 
	{
		std::type_index typeIndex{ typeid(ComponentType) };

		for (size_t i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i]->GetType() == typeIndex)
			{
				return std::dynamic_pointer_cast<ComponentType>(m_Components[i]);
			}
		}

		return nullptr;
	}
	template<DerivedFromComponent ComponentType>
	inline std::vector<std::shared_ptr<ComponentType>> GameObject::GetComponentHandles()
	{
		std::vector<std::shared_ptr<ComponentType>> handles{};
		std::type_index typeIndex{ typeid(ComponentType) };

		for (size_t i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i]->GetType() == typeIndex)
			{
				handles.push_back(std::dynamic_pointer_cast<ComponentType>(m_Components[i]));
			}
		}

		return handles;
	}
	template<DerivedFromComponent ComponentType>
	inline bool GameObject::SetComponent(const ComponentType& component) const
	{
		std::type_index typeIndex{ typeid(ComponentType) };

		for (size_t i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i]->GetType() == typeIndex)
			{
				*m_Components[i].get() = component;

				return true;
			}
		}

		return false;
	}
}
