#pragma once

#include <memory>
#include <typeindex>

namespace dae {

	class GameObject;
	using GameObjectHandle = std::shared_ptr<GameObject>;

	class Component
	{
	public:
		Component() = default;
		virtual ~Component() = default;
		virtual void Start(){}
		virtual void FixedUpdate() {}
		virtual void Update() {}
		virtual void Render() const {}
		virtual void End(){}
		const GameObject& OwnerConst() const{ return *m_Owner; }
		GameObject& Owner() { return *m_Owner; }
		bool HasOwner()const { return m_Owner != nullptr; };
		virtual bool IsActive()const;
		virtual void SetActive(bool active);

	private:
		 
		void SetOwner(GameObject& owner);
		friend class GameObject;
		GameObject* m_Owner{nullptr};
		bool m_IsActive{ true };
	};

	using ComponentHandle = std::shared_ptr<Component>;
}


