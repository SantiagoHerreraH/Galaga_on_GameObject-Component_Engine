#include "Collider.h"
#include <algorithm>
#include "SceneManager.h"
#include "Scene.h"
#include "Renderer.h"

namespace dae {
	CCollider::CCollider(const Rect& rect, const CollisionTag& tag) :
		m_OriginalRect(rect),
		m_CollisionTag(tag),
		m_TransformedRect(rect)
	{
	}


	void CCollider::CenterRect()
	{
		m_Offset.x = -m_OriginalRect.Width / 2.f;
		m_Offset.y = -m_OriginalRect.Height / 2.f;
	}

	void CCollider::SetRectOffset(const glm::vec2& offset)
	{
		m_Offset = offset;
	}

	void CCollider::SetOriginalRect(const Rect& rect)
	{
		m_OriginalRect = rect;
	}
	
	const Rect& CCollider::GetOriginalRect() const
	{
		return m_OriginalRect;
	}
	const Rect& CCollider::GetTransformedRect() const
	{
		TransformRect();
		return m_TransformedRect;
	}
	CollisionTag CCollider::GetCollisionTag()
	{
		return m_CollisionTag;
	}
	
	void CCollider::AddCollisionTagToCollideWith(const CollisionTag& tag)
	{
		//probably not a lot, so it's okay to do this
		for (size_t i = 0; i < m_CollisionTagsToCollideWith.size(); i++)
		{
			if (m_CollisionTagsToCollideWith[i] == tag)
			{
				return;
			}
		}

		m_CollisionTagsToCollideWith.push_back(tag);
	}
	void CCollider::RenderCollider(bool render)
	{
		m_RenderCollider = render;
	}
	Event<const GameObject&, const GameObject&>& CCollider::OnCollisionBeginEvent()
	{
		return m_OnCollisionBeginEvent;
	}
	Event<const GameObject&, const GameObject&>& CCollider::OnCollisionStayEvent()
	{
		return m_OnCollisionStayEvent;
	}
	Event<const GameObject&, const GameObject&>& CCollider::OnCollisionEndEvent()
	{
		return m_OnCollisionEndEvent;
	}
	
	void CCollider::TransformRect() const
	{
		glm::vec3 pos = OwnerConst().TransformConst().GetWorldTransform().Position;

		m_TransformedRect.Left = m_OriginalRect.Left + m_Offset.x + pos.x; //FINISH THIS
		m_TransformedRect.Top = m_OriginalRect.Top + m_Offset.y + pos.y; //FINISH THIS
	}
	bool CCollider::IsStayOverlappingWith(const GameObjectHandle& other)
	{
		for (size_t i = 0; i < m_CollidingWithEntities.size(); i++)
		{
			if (m_CollidingWithEntities[i] == other)
			{
				return true;
			}
		}
		return false;
	}
	void CCollider::Start()
	{
		std::vector<std::shared_ptr<CCollider>> sceneColliders = SceneManager::GetInstance().GetCurrentScene().GetAllComponentsOfType<CCollider>();

		m_CollidersToCollideWith.clear();

		for (size_t colliderIdx = 0; colliderIdx < sceneColliders.size(); colliderIdx++)
		{
			for (size_t tagIdx = 0; tagIdx < m_CollisionTagsToCollideWith.size(); tagIdx++)
			{
				if (sceneColliders[colliderIdx].get() == this)
				{
					continue;
				}

				if (sceneColliders[colliderIdx]->GetCollisionTag() == m_CollisionTagsToCollideWith[tagIdx])
				{
					m_CollidersToCollideWith.push_back(sceneColliders[colliderIdx]);
					break;
				}
			}
		}
	}
	void CCollider::FixedUpdate()
	{
		const GameObject& owner{ OwnerConst() };
		const Rect& transformedRect{ GetTransformedRect() };
		bool isStayOverlapping{};
		for (size_t i = 0; i < m_CollidersToCollideWith.size(); i++)
		{
			if (m_CollidersToCollideWith[i]->IsActive())
			{
				const GameObject& otherOwner = m_CollidersToCollideWith[i]->OwnerConst();
				const GameObjectHandle& otherOwnerHandle = m_CollidersToCollideWith[i]->OwnerHandleConst();
				isStayOverlapping = IsStayOverlappingWith(m_CollidersToCollideWith[i]->OwnerHandleConst());

				if (IsColliding(transformedRect, m_CollidersToCollideWith[i]->GetTransformedRect()))
				{
					if (!isStayOverlapping)
					{
						m_OnCollisionBeginEvent.Invoke(owner, otherOwner);

						m_CollidingWithEntities.push_back(otherOwnerHandle);
						m_CollidersToCollideWith[i]->m_CollidingWithEntities.push_back(OwnerHandle());
					}

					m_OnCollisionStayEvent.Invoke(owner, otherOwner);
				}
				else if(isStayOverlapping)
				{
					m_OnCollisionEndEvent.Invoke(owner, otherOwner);

					std::erase(m_CollidingWithEntities, otherOwnerHandle);
					std::erase(m_CollidersToCollideWith[i]->m_CollidingWithEntities, OwnerHandle());

				}
				
			}
		}

	}
	void CCollider::Render() const
	{
		if (m_RenderCollider)
		{
			Renderer::GetInstance().DrawRect(m_TransformedRect.Left, m_TransformedRect.Top, m_TransformedRect.Width, m_TransformedRect.Height, false);
		}
	}
}


