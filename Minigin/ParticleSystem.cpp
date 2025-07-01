#include "ParticleSystem.h"
#include "Renderer.h"
#include "Time.h"
#include "Random.hpp"

void dae::CParticleSystem::Render() const
{
	for (size_t i = 0; i < m_Particles.size(); i++)
	{
		Renderer::GetInstance().DrawDot(m_Particles[i].Position, m_Particles[i].Color);
	}
}

void dae::CParticleSystem::Update()
{
	for (auto& particle : m_Particles)
	{
		particle.Position += glm::normalize(particle.MovementDirection) * particle.MovementSpeed * Time::GetInstance().GetElapsedSeconds();

		if (m_ParticleRules.HasBounds)
		{
			particle.Position.x =
				particle.Position.x < m_ParticleRules.Bounds.Left ?    m_ParticleRules.Bounds.Right() :
				particle.Position.x > m_ParticleRules.Bounds.Right() ? m_ParticleRules.Bounds.Left : particle.Position.x;

			particle.Position.y =
				particle.Position.y < m_ParticleRules.Bounds.Top ?		m_ParticleRules.Bounds.Bottom() :
				particle.Position.y > m_ParticleRules.Bounds.Bottom() ? m_ParticleRules.Bounds.Top : particle.Position.y;
		}
	}
}

void dae::CParticleSystem::SetNumber(size_t particleNumber)
{
	size_t lastSize = m_Particles.size();

	m_Particles.resize(particleNumber);

	for (size_t i = lastSize; i < m_Particles.size(); i++)
	{
		m_Particles[i].Color = Random::GetRandomBetweenRange(m_ParticleRules.fromColor, m_ParticleRules.toColor);
		m_Particles[i].MovementDirection = Random::GetRandomBetweenRange(m_ParticleRules.fromMovementDirection, m_ParticleRules.toMovementDirection);
		m_Particles[i].MovementSpeed = Random::GetRandomBetweenRange(m_ParticleRules.fromMovementSpeed, m_ParticleRules.toMovementSpeed);
		m_Particles[i].Position = Random::GetRandomBetweenRange(m_ParticleRules.fromPosition, m_ParticleRules.toPosition);
	}
}

void dae::CParticleSystem::SetColorRange(const SDL_Color& from, const SDL_Color& to)
{
	m_ParticleRules.fromColor = from;
	m_ParticleRules.toColor = to;

	for (size_t i = 0; i < m_Particles.size(); i++)
	{
		m_Particles[i].Color = Random::GetRandomBetweenRange(m_ParticleRules.fromColor, m_ParticleRules.toColor);
	}
}

void dae::CParticleSystem::SetMovementDirectionRange(const glm::vec2& from, const glm::vec2& to)
{

	m_ParticleRules.fromMovementDirection = from;
	m_ParticleRules.toMovementDirection = to;

	for (size_t i = 0; i < m_Particles.size(); i++)
	{
		m_Particles[i].MovementDirection = Random::GetRandomBetweenRange(m_ParticleRules.fromMovementDirection, m_ParticleRules.toMovementDirection);
	}
}

void dae::CParticleSystem::SetPositionRange(const glm::ivec2& from, const glm::ivec2& to)
{
	m_ParticleRules.fromPosition = from;
	m_ParticleRules.toPosition = to;

	for (size_t i = 0; i < m_Particles.size(); i++)
	{
		m_Particles[i].Position = Random::GetRandomBetweenRange(m_ParticleRules.fromPosition, m_ParticleRules.toPosition);
	}
}

void dae::CParticleSystem::SetMovementSpeedRange(float from, float to)
{
	m_ParticleRules.fromMovementSpeed = from;
	m_ParticleRules.toMovementSpeed = to;

	for (size_t i = 0; i < m_Particles.size(); i++)
	{
		m_Particles[i].MovementSpeed = Random::GetRandomBetweenRange(m_ParticleRules.fromMovementSpeed, m_ParticleRules.toMovementSpeed);
	}
}

void dae::CParticleSystem::SetBounds(bool hasBounds, const Rect& bounds)
{
	m_ParticleRules.HasBounds = hasBounds;
	m_ParticleRules.Bounds = bounds;
}

