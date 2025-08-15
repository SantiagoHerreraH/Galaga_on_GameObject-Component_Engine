#pragma once
#include <memory>
#include <string>
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
		GameObject(const std::string& name = "");
		virtual ~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		void SetName(const std::string& name) { m_Name = name; }
		const std::string& GetName()const { return m_Name; }

		bool AlreadyStarted()const;

		void Start();
		void Update();
		void FixedUpdate();
		void Render() const;
		void End();

		bool IsActive()const;
		void SetActive(bool active, bool affectChildren = true);

		const dae::Transform& TransformConst()const;
		dae::Transform& Transform();
		void RenderTransform(bool render) { m_RenderTransform = render; }

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
		bool SetComponent(const ComponentType& component);

	private:
		bool m_RenderTransform{ true };
		std::string m_Name;
		dae::Transform m_Transform;
		bool m_IsActive{ true };
		bool m_Started{ false };

		struct ComponentData {
			ComponentHandle ComponentHandle;
			std::type_index Type;
		};

		std::vector<ComponentData> m_Components;

	};

	using GameObjectHandle = std::shared_ptr<GameObject>;
	

	template<DerivedFromComponent ComponentType>
	inline std::shared_ptr<ComponentType> GameObject::AddComponent(const ComponentType& component)
	{
		m_Components.push_back(ComponentData{ std::make_shared<ComponentType>(component) , typeid(ComponentType)});
		m_Components.back().ComponentHandle->SetOwner(*this);

		return std::dynamic_pointer_cast<ComponentType>(m_Components.back().ComponentHandle);
	}

	template<DerivedFromComponent ComponentType>
	inline bool GameObject::HasComponent() const
	{
		std::type_index typeIndex{ typeid(ComponentType) };

		for (size_t i = 0; i < m_Components.size(); i++)
		{
			if (m_Components[i].Type == typeIndex)
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
			if (m_Components[i].Type == typeIndex)
			{
				auto casted = std::dynamic_pointer_cast<ComponentType>(m_Components[i].ComponentHandle);

				return casted.get();
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
			if (m_Components[i].Type == typeInfo)
			{
				return std::dynamic_pointer_cast<ComponentType>(m_Components[i].ComponentHandle).get();
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
			if (m_Components[i].Type == typeIndex)
			{
				return std::dynamic_pointer_cast<ComponentType>(m_Components[i].ComponentHandle);
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
			if (m_Components[i].Type == typeIndex)
			{
				handles.push_back(std::dynamic_pointer_cast<ComponentType>(m_Components[i].ComponentHandle));
			}
		}

		return handles;
	}
	template<DerivedFromComponent ComponentType>
	inline bool GameObject::SetComponent(const ComponentType& component) 
	{
		ComponentType *comp = GetComponent<ComponentType>();
		
		if (comp)
		{
			bool isActive = comp->IsActive();
			*comp = component;
			comp->SetOwner(*this);
			comp->SetActive(isActive);

			if (m_Started)
			{
				comp->Start();
			}

			return true;
		}

		return false;
	}
}
