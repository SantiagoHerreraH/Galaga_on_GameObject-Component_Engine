#pragma once

#include <vector>
#include <glm.hpp>
#include <SDL.h>
#include "Component.h"
#include "Rect.h"

namespace dae {

	struct Particle {
		glm::ivec2 Position;
		glm::vec2 MovementDirection;
		float MovementSpeed;
		SDL_Color Color;
		bool HasBounds{ true };
		Rect Bounds;
	};

	class CParticleSystem final : public Component
	{

	public:

		void Render() const override;
		void Update() override;

		void SetNumber(size_t particleNumber);
		void SetColorRange(const SDL_Color& from, const SDL_Color& to);
		void SetMovementDirectionRange(const glm::vec2& from, const glm::vec2& to);
		void SetPositionRange(const glm::ivec2& from, const glm::ivec2& to);
		void SetMovementSpeedRange(float from, float to);
		void SetBounds(bool hasBounds, const Rect& bounds = {});


	private:
		struct ParticleRules {
			SDL_Color fromColor,				toColor;
			glm::vec2 fromMovementDirection,	toMovementDirection;
			glm::ivec2 fromPosition,				toPosition;
			float fromMovementSpeed,		toMovementSpeed;

			bool HasBounds{ true };
			Rect Bounds;
		};

		struct Particle {
			glm::ivec2 Position;
			glm::vec2 MovementDirection;
			float MovementSpeed;
			SDL_Color Color;
		};


		ParticleRules m_ParticleRules;
		std::vector<Particle> m_Particles;
	};
}



