#pragma once

#include "GameObject.h"

namespace dae {

	class Component
	{
	public:
		Component() = default;
		virtual ~Component() = default;
		virtual void Start(){}
		virtual void FixedUpdate() {}
		virtual void Update() {}
		virtual void Render() const {}
		const GameObject& OwnerConst() const{ return *m_Owner.get(); }
		GameObject& Owner() { return *m_Owner.get(); }
		const GameObjectHandle& OwnerHandleConst() const { return m_Owner; }
		GameObjectHandle& OwnerHandle() { return m_Owner; }
		bool HasOwner()const { return m_Owner != nullptr; };
		bool IsActive()const;
		void SetActive(bool active);

		const std::type_info& GetType() const;

	private:

		void SetOwner(GameObject& owner);
		friend class GameObject;
		GameObjectHandle m_Owner{nullptr};
		bool m_IsActive{ true };
	};


}


