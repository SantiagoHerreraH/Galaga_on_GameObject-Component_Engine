#include "CMovement2D.h"
#include "GameObject.h"

void dae::CMovement2D::Start()
{
	m_Rigidbody = Owner().GetComponentHandle<CRigidbody2D>();
	assert(m_Rigidbody != nullptr);
}

void dae::CMovement2D::FixedUpdate()
{
	if (glm::length(m_MovementInput) > FLT_EPSILON)
	{
		m_MovementInput = glm::normalize(m_MovementInput);
	}

	m_Rigidbody->SetVelocity(m_MovementInput * m_MaxSpeed);
	m_MovementInput = glm::vec2{ 0,0 };
}

void dae::CMovement2D::SetMaxSpeed(float maxSpeed)
{
	m_MaxSpeed = maxSpeed < 0 ? 0 : maxSpeed;
}

void dae::CMovement2D::AddSingleFrameMovementInput(const glm::vec2& input)
{
	m_MovementInput += input;
}

