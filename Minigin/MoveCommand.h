#pragma once

#include <glm.hpp>
#include "Command.h"
#include "Movement.h"

namespace dae {


	class MoveCommand : public ICommand
	{
	public:
		MoveCommand(glm::vec2 direction, float speed) : m_Direction(direction), m_Speed(speed){}
		virtual ~MoveCommand() = default;
		virtual void Execute(GameObject& gameObject) {

			CMovement2D* movement = gameObject.GetComponent<CMovement2D>();

			if (movement)
			{
				movement->SetMaxSpeed(m_Speed);
				movement->AddSingleFrameMovementInput(m_Direction);
			}
		}

	private:
		glm::vec2 m_Direction;
		float m_Speed;
	};

}