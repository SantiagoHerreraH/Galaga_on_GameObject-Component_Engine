#include "Rigidbody.h"
#include "GameTime.h"
#include "GameObject.h"

namespace dae {

#pragma region Component

	void CRigidbody2D::FixedUpdate()
	{
		const float fixedTime{ Time::GetInstance().GetFixedTimeStep() };

		AddVelocity(GetNetAcceleration());
		AddAngularVelocity(GetNetAngularAcceleration());

		Owner().Transform().MoveLocalPositionX(GetVelocityX() * fixedTime);
		Owner().Transform().MoveLocalPositionY(GetVelocityY() * fixedTime);

		Owner().Transform().MoveLocalRotation(GetAngularVelocity() * fixedTime);

		SetNetAcceleration(glm::vec2{}, ForceMode2D::Impulse);//set to impulse just to avoid unnecesary multiiplication
		SetNetAngularAcceleration(glm::vec3{}, ForceMode2D::Impulse);
	}

	void CRigidbody2D::SetNetTorque(const glm::vec3& force, ForceMode2D forceMode2D)
	{
		switch (forceMode2D)
		{
		case dae::ForceMode2D::Force:

			m_NetTorque = force * Time::GetInstance().GetFixedTimeStep();
			m_NetAngularAcceleration = m_NetTorque / m_Mass;
			break;
		case dae::ForceMode2D::Impulse:
			m_NetTorque = force;
			m_NetAngularAcceleration = m_NetTorque / m_Mass;
			break;
		default:
			break;
		}
	}

	void CRigidbody2D::SetNetAngularAcceleration(const glm::vec3& acceleration, ForceMode2D forceMode2D)
	{
		switch (forceMode2D)
		{
		case dae::ForceMode2D::Force:

			m_NetAngularAcceleration = acceleration * Time::GetInstance().GetFixedTimeStep();
			m_NetTorque = m_NetAngularAcceleration * m_Mass;
			break;
		case dae::ForceMode2D::Impulse:
			m_NetAngularAcceleration = acceleration;
			m_NetTorque = m_NetAngularAcceleration * m_Mass;
			break;
		default:
			break;
		}
	}

	void CRigidbody2D::SetNetForce(const glm::vec2& force, ForceMode2D forceMode2D)
	{
		switch (forceMode2D)
		{
		case dae::ForceMode2D::Force:
			m_NetAcceleration = (force/m_Mass) * Time::GetInstance().GetFixedTimeStep();
			m_NetForce = force * Time::GetInstance().GetFixedTimeStep();
			break;
		case dae::ForceMode2D::Impulse:
			m_NetAcceleration = (force / m_Mass);
			m_NetForce = force;
			break;
		default:
			break;
		}
	}

	void CRigidbody2D::SetNetAcceleration(const glm::vec2& acceleration, ForceMode2D forceMode2D)
	{
		switch (forceMode2D)
		{
		case dae::ForceMode2D::Force:

			m_NetAcceleration = acceleration * Time::GetInstance().GetFixedTimeStep();
			m_NetForce = m_NetAcceleration * m_Mass;
			break;
		case dae::ForceMode2D::Impulse:
			m_NetAcceleration = acceleration;
			m_NetForce = m_NetAcceleration * m_Mass;
			break;
		default:
			break;
		}
	}

	void CRigidbody2D::AddTorque(const glm::vec3& force, ForceMode2D forceMode2D)
	{
		switch (forceMode2D)
		{
		case dae::ForceMode2D::Force:
			m_NetTorque += force * Time::GetInstance().GetFixedTimeStep();
			m_NetAngularAcceleration += (force/m_Mass) * Time::GetInstance().GetFixedTimeStep();

			break;
		case dae::ForceMode2D::Impulse:
			m_NetTorque += force;
			m_NetAngularAcceleration += (force / m_Mass);
			break;
		default:
			break;
		}
	}

	void CRigidbody2D::AddForce(const glm::vec2& force, ForceMode2D forceMode2D)
	{
		switch (forceMode2D)
		{
		case dae::ForceMode2D::Force:
			m_NetAcceleration += (force / m_Mass) * Time::GetInstance().GetFixedTimeStep();
			m_NetForce += force * Time::GetInstance().GetFixedTimeStep();
			break;
		case dae::ForceMode2D::Impulse:
			m_NetAcceleration += (force / m_Mass);
			m_NetForce += force;
			break;
		default:
			break;
		}
	}

	void CRigidbody2D::AddAcceleration(const glm::vec2& acceleration, ForceMode2D forceMode2D)
	{
		switch (forceMode2D)
		{
		case dae::ForceMode2D::Force:

			m_NetAcceleration += acceleration * Time::GetInstance().GetFixedTimeStep();
			m_NetForce += m_NetAcceleration * m_Mass;
			break;
		case dae::ForceMode2D::Impulse:
			m_NetAcceleration += acceleration;
			m_NetForce += m_NetAcceleration * m_Mass;
			break;
		default:
			break;
		}

	}

	void CRigidbody2D::AddVelocity(const glm::vec2& velocity, ForceMode2D forceMode2D)
	{
		switch (forceMode2D)
		{
		case dae::ForceMode2D::Force:
			m_LinearVelocity += velocity * Time::GetInstance().GetFixedTimeStep();
			break;
		case dae::ForceMode2D::Impulse:			
			m_LinearVelocity += velocity;
			break;
		default:
			break;
		}
	}

	void CRigidbody2D::AddAngularVelocity(const glm::vec3& velocity, ForceMode2D forceMode2D)
	{
		switch (forceMode2D)
		{
		case dae::ForceMode2D::Force:
			m_AngularVelocity += velocity * Time::GetInstance().GetFixedTimeStep();
			break;
		case dae::ForceMode2D::Impulse:
			m_AngularVelocity += velocity;
			break;
		default:
			break;
		}
	}

	const glm::vec3& CRigidbody2D::GetNetTorque() const
	{
		return m_NetTorque;
	}

	const glm::vec3& CRigidbody2D::GetNetAngularAcceleration() const
	{
		return m_NetAngularAcceleration;
	}

	const glm::vec2& CRigidbody2D::GetNetAcceleration() const
	{
		return m_NetAcceleration;
	}

	const glm::vec2& CRigidbody2D::GetNetForce() const
	{
		return m_NetForce;
	}

	float CRigidbody2D::GetMass() const
	{
		return m_Mass;
	}

	void CRigidbody2D::SetMass(float mass)
	{
		m_Mass = mass > 0 ? mass : 0.1f;
	}

	void CRigidbody2D::ClampVelocity(float maxMagnitude, SetVelocityMode setVelocityModeForXAxis)
	{
		float currentLength = glm::length(m_LinearVelocity);
		if (currentLength > maxMagnitude && 
			currentLength > FLT_EPSILON &&
			maxMagnitude > FLT_EPSILON)
		{
			switch (setVelocityModeForXAxis)
			{
			case dae::SetVelocityMode::NullForces:

				m_LinearVelocity = glm::normalize(m_LinearVelocity) * maxMagnitude;
				m_NetAcceleration.x = 0;
				m_NetForce.x = 0;
				break;
			case dae::SetVelocityMode::KeepForces:
				m_LinearVelocity = glm::normalize(m_LinearVelocity) * maxMagnitude;
				break;
			default:
				break;
			}
		}
		
	}

	void CRigidbody2D::SetVelocityX(float velX, SetVelocityMode setVelocityModeForXAxis)
	{
		switch (setVelocityModeForXAxis)
		{
		case dae::SetVelocityMode::NullForces:
			m_LinearVelocity.x = velX;
			m_NetAcceleration.x = 0;
			m_NetForce.x = 0;
			break;
		case dae::SetVelocityMode::KeepForces:
			m_LinearVelocity.x = velX;
			break;
		default:
			break;
		}
	}

	void CRigidbody2D::SetVelocityY(float velY, SetVelocityMode setVelocityModeForYAxis)
	{
		switch (setVelocityModeForYAxis)
		{
		case dae::SetVelocityMode::NullForces:
			m_LinearVelocity.y = velY;
			m_NetAcceleration.y = 0;
			m_NetForce.y = 0;
			break;
		case dae::SetVelocityMode::KeepForces:
			m_LinearVelocity.y = velY;
			break;
		default:
			break;
		}
	}

	void CRigidbody2D::SetVelocity(const glm::vec2& vel, SetVelocityMode setVelocityMode)
	{
		switch (setVelocityMode)
		{
		case dae::SetVelocityMode::NullForces:
			m_LinearVelocity = vel;
			m_NetAcceleration = glm::vec2{};
			m_NetForce = glm::vec2{};
			break;
		case dae::SetVelocityMode::KeepForces:
			m_LinearVelocity = vel;
			break;
		default:
			break;
		}
	}

	float CRigidbody2D::GetVelocityX() const
	{
		return m_LinearVelocity.x;
	}

	float CRigidbody2D::GetVelocityY() const
	{
		return m_LinearVelocity.y;
	}

	const glm::vec2& CRigidbody2D::GetVelocity() const
	{
		return m_LinearVelocity;
	}

	float CRigidbody2D::GetAngularVelocityX() const
	{
		return m_AngularVelocity.x;
	}

	float CRigidbody2D::GetAngularVelocityY() const
	{
		return m_AngularVelocity.y;
	}

	float CRigidbody2D::GetAngularVelocityZ() const
	{
		return m_AngularVelocity.z;
	}

	const glm::vec3& CRigidbody2D::GetAngularVelocity() const
	{
		return m_AngularVelocity;
	}

#pragma endregion

}