#pragma once

#include <glm.hpp>
#include "ICommand.h"
#include "CMovement2D.h"
#include "Rect.h"

namespace dae {


	class MoveCommand : public ICommand
	{
	public:
		MoveCommand(glm::vec2 direction, const Rect& bounds) : m_Bounds(bounds), m_Direction(direction){}
		virtual ~MoveCommand() = default;
		virtual void Execute(GameObject& gameObject) {

			CMovement2D* movement = gameObject.GetComponent<CMovement2D>();
			glm::vec2 pos = gameObject.Transform().GetWorldTransform().Position;

			if (movement)
			{
				if (m_Bounds.IsInBounds( pos.x, pos.y))
				{
					movement->AddSingleFrameMovementInput(m_Direction);
				}
				else
				{
					pos = m_Bounds.KeepInBounds(pos);
					gameObject.Transform().SetLocalPositionX(pos.x);
					gameObject.Transform().SetLocalPositionY(pos.y);
				}
			}
		}

	private:
		Rect m_Bounds{};
		glm::vec2 m_Direction{};
	};

}