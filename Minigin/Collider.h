#pragma once

#include <string>
#include "Component.h"
#include "EventSystem.h"
#include "Rect.h"

namespace dae {

	using CollisionTag = int;

	class CCollider final : public Component
	{
	public:
		CCollider(const Rect& rect, const CollisionTag& tag);

		void Start() override;
		void FixedUpdate() override;
		void Render()const override;

		void SetRectOffset(const glm::vec2& offset);
		void CenterRect();

		void SetOriginalRect(const Rect& rect);
		const Rect& GetOriginalRect()const;
		const Rect& GetTransformedRect()const;

		CollisionTag GetCollisionTag();
		void AddCollisionTagToCollideWith(const CollisionTag& tag);
		
		void RenderCollider(bool render);

		Event<const GameObject&, const GameObject&>& OnCollisionBeginEvent();
		Event<const GameObject&, const GameObject&>& OnCollisionStayEvent();
		Event<const GameObject&, const GameObject&>& OnCollisionEndEvent();

	private:
		glm::vec2 m_Offset;
		Rect m_OriginalRect{};
		mutable Rect m_TransformedRect{};
		void TransformRect()const;

		CollisionTag m_CollisionTag{-1};//default collision tag
		std::vector<CollisionTag> m_CollisionTagsToCollideWith{};

		bool m_RenderCollider{ false };
		std::vector<std::shared_ptr<CCollider>> m_CollidersToCollideWith;

		bool IsStayOverlappingWith(const GameObjectHandle& other);
		std::vector<GameObjectHandle> m_CollidingWithEntities;

		Event<const GameObject&, const GameObject&> m_OnCollisionStayEvent{ }; //self, other
		Event<const GameObject&, const GameObject&> m_OnCollisionBeginEvent{ }; //self, other
		Event<const GameObject&, const GameObject&> m_OnCollisionEndEvent{ }; //self, other 
	};

}

