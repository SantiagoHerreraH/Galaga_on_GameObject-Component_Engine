#pragma once
#include <memory>
#include "Transform.h"

namespace dae
{
	class Component;
	using ComponentHandle = std::shared_ptr<Component>;

	template<typename T>
	concept DerivedFromComponent = std::derived_from<T, Component>;

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
		void SetActive(bool active);

		const dae::Transform& TransformConst()const;
		dae::Transform& Transform();

		void AddComponent(const Component& component);
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
	inline bool GameObject::HasComponent() const
	{
		std::type_info typeInfo{ typeid(ComponentType) };

		for (size_t i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i]->GetType() == typeInfo)
			{
				return true;
			}
		}

		return false;
	}

	template<DerivedFromComponent ComponentType>
	inline ComponentType* GameObject::GetComponent()
	{
		std::type_info typeInfo{ typeid(ComponentType) };

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
	inline const ComponentType* const GameObject::GetComponentConst() const
	{
		std::type_info typeInfo{ typeid(ComponentType) };

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
		std::type_info typeInfo{ typeid(ComponentType) };

		for (size_t i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i]->GetType() == typeInfo)
			{
				return m_Components[i];
			}
		}

		return nullptr;
	}
	template<DerivedFromComponent ComponentType>
	inline std::vector<std::shared_ptr<ComponentType>> GameObject::GetComponentHandles()
	{
		std::vector<ComponentHandle> handles{};
		std::type_info typeInfo{ typeid(ComponentType) };

		for (size_t i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i]->GetType() == typeInfo)
			{
				handles.push_back(m_Components[i]);
			}
		}

		return handles;
	}
	template<DerivedFromComponent ComponentType>
	inline bool GameObject::SetComponent(const ComponentType& component) const
	{
		std::type_info typeInfo{ typeid(ComponentType) };

		for (size_t i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i]->GetType() == typeInfo)
			{
				*m_Components[i].get() = component;

				return true;
			}
		}

		return false;
	}
}
